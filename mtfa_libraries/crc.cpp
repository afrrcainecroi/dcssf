#include <iostream>
#include <string.h>
#include <stdlib.h>

#include <string>
#include "crc.h"

using namespace std;


unsigned  int CrcClass::crc ( unsigned char data )
{
     unsigned char * p = new unsigned char[8];

     memset ( p, 0x7F, 8 );
     p[0]=data;
     unsigned  int uRet = wombat ( p, 8 );

     delete [] p;
     return uRet;
};

unsigned  int CrcClass::crc ( unsigned short data )
{
     unsigned char * p = new  unsigned char[8];
     memset ( p, 0x7F, 8 );
     p[0]=data & 0x00FF;
     p[1] = ( data >> 8 ) & 0x00FF;
     unsigned  int uRet = wombat ( p, 8 );

     delete [] p;

     return uRet;
};

unsigned  int CrcClass::crc ( unsigned int data )
{
     unsigned char * p = new unsigned char[8];
     memset ( p, 0x7F, 8 );
     p[0]=data & 0x000000FF;
     p[1] = ( data >> 8 ) & 0x000000FF;
     p[2] = ( data >> 16 ) & 0x000000FF;
     p[3] = ( data >> 24 ) & 0x000000FF;
     unsigned  int uRet = wombat ( p, 8 );

     delete [] p;

     return uRet;
};

unsigned  int CrcClass::crc ( unsigned char *data, int len )
{
     if ( len < 8 ) {
          unsigned char * p = new unsigned char[8];
          memset ( p, 0x7F, 8 );
          for ( int i=0; i<len; i++ ) {
               p[i]=data[i];
          }
          unsigned  int uRet = wombat ( p, 8 );
          delete [] p;

          return uRet;
     } else {
          return wombat ( data, len );
     }
};

unsigned  int CrcClass::crc ( char data )
{
     return crc ( ( unsigned char ) data );
};
unsigned  int CrcClass::crc ( short data )
{
     return crc ( ( unsigned short ) data );
};
unsigned  int CrcClass::crc ( int data )
{
     return crc ( ( unsigned int ) data );
};
unsigned  int CrcClass::crc ( char *data, int len )
{
     return crc ( ( unsigned char* ) data, len );
};

CrcClass::CrcClass ()
{
     int i, j;
     unsigned int crc;

     for ( i = 0; i < 256; i++ ) {
          crc = i << 24;
          for ( j = 0; j < 8; j++ ) {
               if ( crc & 0x80000000 ) {
                    crc = ( crc << 1 ) ^ QUOTIENT;
               } else {
                    crc = crc << 1;
               }
          }
          crctab[i] = crc;
     }
}



unsigned  int CrcClass::wombat ( unsigned char *data, int len )
{
     unsigned  int        result;
     int                 i;
//    unsigned char       octet;

     if ( len < 4 ) {
          abort();
     }

     result = *data++ << 24;
     result |= *data++ << 16;
     result |= *data++ << 8;
     result |= *data++;
     result = ~ result;
     len -=4;

     for ( i=0; i<len; i++ ) {
          result = ( result << 8 | *data++ ) ^ crctab[result >> 24];
     }

     return ~result;
}



unsigned int av_crc::crc_full ( const string & buffer )
{
     return av_crc::crc_full ( ( const unsigned char* ) ( buffer.c_str() ), buffer.size() );
}

#define le2me_32(x) (x)

unsigned int av_crc::crc_full ( const unsigned char *buffer, int length )
{
     const unsigned char *end = buffer+length;
     unsigned  int crc = ~0;

     if ( !av_crc::ctx[256] )
          while ( buffer < ( end-3 ) ) {
               crc ^= le2me_32 ( * ( unsigned int* ) buffer );
               buffer+=4;
               crc =  ctx[3*256 + ( crc     &0xFF )]
                      ^ctx[2*256 + ( ( crc>>8 ) &0xFF )]
                      ^ctx[1*256 + ( ( crc>>16 ) &0xFF )]
                      ^ctx[0*256 + ( ( crc>>24 ) &0xFF )];
          }

     while ( buffer<end ) {
          crc = ctx[ ( ( unsigned char ) crc ) ^ *buffer++] ^ ( crc >> 8 );
     }

     return crc;
}

unsigned int av_crc::crc_continue ( unsigned int crc, const unsigned char *buffer, int length )
{
     const unsigned char *end = buffer+length;

     if ( !av_crc::ctx[256] )
          while ( buffer < ( end-3 ) ) {
               crc ^= le2me_32 ( * ( unsigned int* ) buffer );
               buffer+=4;
               crc =  ctx[3*256 + ( crc     &0xFF )]
                      ^ctx[2*256 + ( ( crc>>8 ) &0xFF )]
                      ^ctx[1*256 + ( ( crc>>16 ) &0xFF )]
                      ^ctx[0*256 + ( ( crc>>24 ) &0xFF )];
          }
     while ( buffer<end ) {
          crc = ctx[ ( ( unsigned char ) crc ) ^ *buffer++] ^ ( crc >> 8 );
     }

     return crc;
}

/** This polynomial DOES generate the same CRC values as ZMODEM and PKZIP */
unsigned  int av_crc::ctx[]= {
     0UL, 1996959894UL, 3993919788UL, 2567524794UL, 124634137UL, 1886057615UL, 3915621685UL, 2657392035UL,
     249268274UL, 2044508324UL, 3772115230UL, 2547177864UL, 162941995UL, 2125561021UL, 3887607047UL, 2428444049UL,
     498536548UL, 1789927666UL, 4089016648UL, 2227061214UL, 450548861UL, 1843258603UL, 4107580753UL, 2211677639UL,
     325883990UL, 1684777152UL, 4251122042UL, 2321926636UL, 335633487UL, 1661365465UL, 4195302755UL, 2366115317UL,
     997073096UL, 1281953886UL, 3579855332UL, 2724688242UL, 1006888145UL, 1258607687UL, 3524101629UL, 2768942443UL,
     901097722UL, 1119000684UL, 3686517206UL, 2898065728UL, 853044451UL, 1172266101UL, 3705015759UL, 2882616665UL,
     651767980UL, 1373503546UL, 3369554304UL, 3218104598UL, 565507253UL, 1454621731UL, 3485111705UL, 3099436303UL,
     671266974UL, 1594198024UL, 3322730930UL, 2970347812UL, 795835527UL, 1483230225UL, 3244367275UL, 3060149565UL,
     1994146192UL, 31158534UL, 2563907772UL, 4023717930UL, 1907459465UL, 112637215UL, 2680153253UL, 3904427059UL,
     2013776290UL, 251722036UL, 2517215374UL, 3775830040UL, 2137656763UL, 141376813UL, 2439277719UL, 3865271297UL,
     1802195444UL, 476864866UL, 2238001368UL, 4066508878UL, 1812370925UL, 453092731UL, 2181625025UL, 4111451223UL,
     1706088902UL, 314042704UL, 2344532202UL, 4240017532UL, 1658658271UL, 366619977UL, 2362670323UL, 4224994405UL,
     1303535960UL, 984961486UL, 2747007092UL, 3569037538UL, 1256170817UL, 1037604311UL, 2765210733UL, 3554079995UL,
     1131014506UL, 879679996UL, 2909243462UL, 3663771856UL, 1141124467UL, 855842277UL, 2852801631UL, 3708648649UL,
     1342533948UL, 654459306UL, 3188396048UL, 3373015174UL, 1466479909UL, 544179635UL, 3110523913UL, 3462522015UL,
     1591671054UL, 702138776UL, 2966460450UL, 3352799412UL, 1504918807UL, 783551873UL, 3082640443UL, 3233442989UL,
     3988292384UL, 2596254646UL, 62317068UL, 1957810842UL, 3939845945UL, 2647816111UL, 81470997UL, 1943803523UL,
     3814918930UL, 2489596804UL, 225274430UL, 2053790376UL, 3826175755UL, 2466906013UL, 167816743UL, 2097651377UL,
     4027552580UL, 2265490386UL, 503444072UL, 1762050814UL, 4150417245UL, 2154129355UL, 426522225UL, 1852507879UL,
     4275313526UL, 2312317920UL, 282753626UL, 1742555852UL, 4189708143UL, 2394877945UL, 397917763UL, 1622183637UL,
     3604390888UL, 2714866558UL, 953729732UL, 1340076626UL, 3518719985UL, 2797360999UL, 1068828381UL, 1219638859UL,
     3624741850UL, 2936675148UL, 906185462UL, 1090812512UL, 3747672003UL, 2825379669UL, 829329135UL, 1181335161UL,
     3412177804UL, 3160834842UL, 628085408UL, 1382605366UL, 3423369109UL, 3138078467UL, 570562233UL, 1426400815UL,
     3317316542UL, 2998733608UL, 733239954UL, 1555261956UL, 3268935591UL, 3050360625UL, 752459403UL, 1541320221UL,
     2607071920UL, 3965973030UL, 1969922972UL, 40735498UL, 2617837225UL, 3943577151UL, 1913087877UL, 83908371UL,
     2512341634UL, 3803740692UL, 2075208622UL, 213261112UL, 2463272603UL, 3855990285UL, 2094854071UL, 198958881UL,
     2262029012UL, 4057260610UL, 1759359992UL, 534414190UL, 2176718541UL, 4139329115UL, 1873836001UL, 414664567UL,
     2282248934UL, 4279200368UL, 1711684554UL, 285281116UL, 2405801727UL, 4167216745UL, 1634467795UL, 376229701UL,
     2685067896UL, 3608007406UL, 1308918612UL, 956543938UL, 2808555105UL, 3495958263UL, 1231636301UL, 1047427035UL,
     2932959818UL, 3654703836UL, 1088359270UL, 936918000UL, 2847714899UL, 3736837829UL, 1202900863UL, 817233897UL,
     3183342108UL, 3401237130UL, 1404277552UL, 615818150UL, 3134207493UL, 3453421203UL, 1423857449UL, 601450431UL,
     3009837614UL, 3294710456UL, 1567103746UL, 711928724UL, 3020668471UL, 3272380065UL, 1510334235UL, 755167117UL,
     0UL, 421212481UL, 842424962UL, 724390851UL, 1684849924UL, 2105013317UL, 1448781702UL, 1329698503UL,
     3369699848UL, 3519200073UL, 4210026634UL, 3824474571UL, 2897563404UL, 3048111693UL, 2659397006UL, 2274893007UL,
     1254232657UL, 1406739216UL, 2029285587UL, 1643069842UL, 783210325UL, 934667796UL, 479770071UL, 92505238UL,
     2182846553UL, 2600511768UL, 2955803355UL, 2838940570UL, 3866582365UL, 4285295644UL, 3561045983UL, 3445231262UL,
     2508465314UL, 2359236067UL, 2813478432UL, 3198777185UL, 4058571174UL, 3908292839UL, 3286139684UL, 3670389349UL,
     1566420650UL, 1145479147UL, 1869335592UL, 1987116393UL, 959540142UL, 539646703UL, 185010476UL, 303839341UL,
     3745920755UL, 3327985586UL, 3983561841UL, 4100678960UL, 3140154359UL, 2721170102UL, 2300350837UL, 2416418868UL,
     396344571UL, 243568058UL, 631889529UL, 1018359608UL, 1945336319UL, 1793607870UL, 1103436669UL, 1490954812UL,
     4034481925UL, 3915546180UL, 3259968903UL, 3679722694UL, 2484439553UL, 2366552896UL, 2787371139UL, 3208174018UL,
     950060301UL, 565965900UL, 177645455UL, 328046286UL, 1556873225UL, 1171730760UL, 1861902987UL, 2011255754UL,
     3132841300UL, 2745199637UL, 2290958294UL, 2442530455UL, 3738671184UL, 3352078609UL, 3974232786UL, 4126854035UL,
     1919080284UL, 1803150877UL, 1079293406UL, 1498383519UL, 370020952UL, 253043481UL, 607678682UL, 1025720731UL,
     1711106983UL, 2095471334UL, 1472923941UL, 1322268772UL, 26324643UL, 411738082UL, 866634785UL, 717028704UL,
     2904875439UL, 3024081134UL, 2668790573UL, 2248782444UL, 3376948395UL, 3495106026UL, 4219356713UL, 3798300520UL,
     792689142UL, 908347575UL, 487136116UL, 68299317UL, 1263779058UL, 1380486579UL, 2036719216UL, 1618931505UL,
     3890672638UL, 4278043327UL, 3587215740UL, 3435896893UL, 2206873338UL, 2593195963UL, 2981909624UL, 2829542713UL,
     998479947UL, 580430090UL, 162921161UL, 279890824UL, 1609522511UL, 1190423566UL, 1842954189UL, 1958874764UL,
     4082766403UL, 3930137346UL, 3245109441UL, 3631694208UL, 2536953671UL, 2385372678UL, 2768287173UL, 3155920004UL,
     1900120602UL, 1750776667UL, 1131931800UL, 1517083097UL, 355290910UL, 204897887UL, 656092572UL, 1040194781UL,
     3113746450UL, 2692952403UL, 2343461520UL, 2461357009UL, 3723805974UL, 3304059991UL, 4022511508UL, 4141455061UL,
     2919742697UL, 3072101800UL, 2620513899UL, 2234183466UL, 3396041197UL, 3547351212UL, 4166851439UL, 3779471918UL,
     1725839073UL, 2143618976UL, 1424512099UL, 1307796770UL, 45282277UL, 464110244UL, 813994343UL, 698327078UL,
     3838160568UL, 4259225593UL, 3606301754UL, 3488152955UL, 2158586812UL, 2578602749UL, 2996767038UL, 2877569151UL,
     740041904UL, 889656817UL, 506086962UL, 120682355UL, 1215357364UL, 1366020341UL, 2051441462UL, 1667084919UL,
     3422213966UL, 3538019855UL, 4190942668UL, 3772220557UL, 2945847882UL, 3062702859UL, 2644537544UL, 2226864521UL,
     52649286UL, 439905287UL, 823476164UL, 672009861UL, 1733269570UL, 2119477507UL, 1434057408UL, 1281543041UL,
     2167981343UL, 2552493150UL, 3004082077UL, 2853541596UL, 3847487515UL, 4233048410UL, 3613549209UL, 3464057816UL,
     1239502615UL, 1358593622UL, 2077699477UL, 1657543892UL, 764250643UL, 882293586UL, 532408465UL, 111204816UL,
     1585378284UL, 1197851309UL, 1816695150UL, 1968414767UL, 974272232UL, 587794345UL, 136598634UL, 289367339UL,
     2527558116UL, 2411481253UL, 2760973158UL, 3179948583UL, 4073438432UL, 3956313505UL, 3237863010UL, 3655790371UL,
     347922877UL, 229101820UL, 646611775UL, 1066513022UL, 1892689081UL, 1774917112UL, 1122387515UL, 1543337850UL,
     3697634229UL, 3313392372UL, 3998419255UL, 4148705398UL, 3087642289UL, 2702352368UL, 2319436851UL, 2468674930UL,
     0UL, 29518391UL, 59036782UL, 38190681UL, 118073564UL, 114017003UL, 76381362UL, 89069189UL,
     236147128UL, 265370511UL, 228034006UL, 206958561UL, 152762724UL, 148411219UL, 178138378UL, 190596925UL,
     472294256UL, 501532999UL, 530741022UL, 509615401UL, 456068012UL, 451764635UL, 413917122UL, 426358261UL,
     305525448UL, 334993663UL, 296822438UL, 275991697UL, 356276756UL, 352202787UL, 381193850UL, 393929805UL,
     944588512UL, 965684439UL, 1003065998UL, 973863097UL, 1061482044UL, 1049003019UL, 1019230802UL, 1023561829UL,
     912136024UL, 933002607UL, 903529270UL, 874031361UL, 827834244UL, 815125939UL, 852716522UL, 856752605UL,
     611050896UL, 631869351UL, 669987326UL, 640506825UL, 593644876UL, 580921211UL, 551983394UL, 556069653UL,
     712553512UL, 733666847UL, 704405574UL, 675154545UL, 762387700UL, 749958851UL, 787859610UL, 792175277UL,
     1889177024UL, 1901651959UL, 1931368878UL, 1927033753UL, 2006131996UL, 1985040171UL, 1947726194UL, 1976933189UL,
     2122964088UL, 2135668303UL, 2098006038UL, 2093965857UL, 2038461604UL, 2017599123UL, 2047123658UL, 2076625661UL,
     1824272048UL, 1836991623UL, 1866005214UL, 1861914857UL, 1807058540UL, 1786244187UL, 1748062722UL, 1777547317UL,
     1655668488UL, 1668093247UL, 1630251878UL, 1625932113UL, 1705433044UL, 1684323811UL, 1713505210UL, 1742760333UL,
     1222101792UL, 1226154263UL, 1263738702UL, 1251046777UL, 1339974652UL, 1310460363UL, 1281013650UL, 1301863845UL,
     1187289752UL, 1191637167UL, 1161842422UL, 1149379777UL, 1103966788UL, 1074747507UL, 1112139306UL, 1133218845UL,
     1425107024UL, 1429406311UL, 1467333694UL, 1454888457UL, 1408811148UL, 1379576507UL, 1350309090UL, 1371438805UL,
     1524775400UL, 1528845279UL, 1499917702UL, 1487177649UL, 1575719220UL, 1546255107UL, 1584350554UL, 1605185389UL,
     3778354048UL, 3774312887UL, 3803303918UL, 3816007129UL, 3862737756UL, 3892238699UL, 3854067506UL, 3833203973UL,
     4012263992UL, 4007927823UL, 3970080342UL, 3982554209UL, 3895452388UL, 3924658387UL, 3953866378UL, 3932773565UL,
     4245928176UL, 4241609415UL, 4271336606UL, 4283762345UL, 4196012076UL, 4225268251UL, 4187931714UL, 4166823541UL,
     4076923208UL, 4072833919UL, 4035198246UL, 4047918865UL, 4094247316UL, 4123732899UL, 4153251322UL, 4132437965UL,
     3648544096UL, 3636082519UL, 3673983246UL, 3678331705UL, 3732010428UL, 3753090955UL, 3723829714UL, 3694611429UL,
     3614117080UL, 3601426159UL, 3572488374UL, 3576541825UL, 3496125444UL, 3516976691UL, 3555094634UL, 3525581405UL,
     3311336976UL, 3298595879UL, 3336186494UL, 3340255305UL, 3260503756UL, 3281337595UL, 3251864226UL, 3222399125UL,
     3410866088UL, 3398419871UL, 3368647622UL, 3372945905UL, 3427010420UL, 3448139075UL, 3485520666UL, 3456284973UL,
     2444203584UL, 2423127159UL, 2452308526UL, 2481530905UL, 2527477404UL, 2539934891UL, 2502093554UL, 2497740997UL,
     2679949304UL, 2659102159UL, 2620920726UL, 2650438049UL, 2562027300UL, 2574714131UL, 2603727690UL, 2599670141UL,
     2374579504UL, 2353749767UL, 2383274334UL, 2412743529UL, 2323684844UL, 2336421851UL, 2298759554UL, 2294686645UL,
     2207933576UL, 2186809023UL, 2149495014UL, 2178734801UL, 2224278612UL, 2236720739UL, 2266437690UL, 2262135309UL,
     2850214048UL, 2820717207UL, 2858812622UL, 2879680249UL, 2934667388UL, 2938704459UL, 2909776914UL, 2897069605UL,
     2817622296UL, 2788420399UL, 2759153014UL, 2780249921UL, 2700618180UL, 2704950259UL, 2742877610UL, 2730399645UL,
     3049550800UL, 3020298727UL, 3057690558UL, 3078802825UL, 2999835404UL, 3004150075UL, 2974355298UL, 2961925461UL,
     3151438440UL, 3121956959UL, 3092510214UL, 3113327665UL, 3168701108UL, 3172786307UL, 3210370778UL, 3197646061UL,
     0UL, 3099354981UL, 2852767883UL, 313896942UL, 2405603159UL, 937357362UL, 627793884UL, 2648127673UL,
     3316918511UL, 2097696650UL, 1874714724UL, 3607201537UL, 1255587768UL, 4067088605UL, 3772741427UL, 1482887254UL,
     1343838111UL, 3903140090UL, 4195393300UL, 1118632049UL, 3749429448UL, 1741137837UL, 1970407491UL, 3452858150UL,
     2511175536UL, 756094997UL, 1067759611UL, 2266550430UL, 449832999UL, 2725482306UL, 2965774508UL, 142231497UL,
     2687676222UL, 412010587UL, 171665333UL, 2995192016UL, 793786473UL, 2548850444UL, 2237264098UL, 1038456711UL,
     1703315409UL, 3711623348UL, 3482275674UL, 1999841343UL, 3940814982UL, 1381529571UL, 1089329165UL, 4166106984UL,
     4029413537UL, 1217896388UL, 1512189994UL, 3802027855UL, 2135519222UL, 3354724499UL, 3577784189UL, 1845280792UL,
     899665998UL, 2367928107UL, 2677414085UL, 657096608UL, 3137160985UL, 37822588UL, 284462994UL, 2823350519UL,
     2601801789UL, 598228824UL, 824021174UL, 2309093331UL, 343330666UL, 2898962447UL, 3195996129UL, 113467524UL,
     1587572946UL, 3860600759UL, 4104763481UL, 1276501820UL, 3519211397UL, 1769898208UL, 2076913422UL, 3279374443UL,
     3406630818UL, 1941006535UL, 1627703081UL, 3652755532UL, 1148164341UL, 4241751952UL, 3999682686UL, 1457141531UL,
     247015245UL, 3053797416UL, 2763059142UL, 470583459UL, 2178658330UL, 963106687UL, 735213713UL, 2473467892UL,
     992409347UL, 2207944806UL, 2435792776UL, 697522413UL, 3024379988UL, 217581361UL, 508405983UL, 2800865210UL,
     4271038444UL, 1177467017UL, 1419450215UL, 3962007554UL, 1911572667UL, 3377213406UL, 3690561584UL, 1665525589UL,
     1799331996UL, 3548628985UL, 3241568279UL, 2039091058UL, 3831314379UL, 1558270126UL, 1314193216UL, 4142438437UL,
     2928380019UL, 372764438UL, 75645176UL, 3158189981UL, 568925988UL, 2572515393UL, 2346768303UL, 861712586UL,
     3982079547UL, 1441124702UL, 1196457648UL, 4293663189UL, 1648042348UL, 3666298377UL, 3358779879UL, 1888390786UL,
     686661332UL, 2421291441UL, 2196002399UL, 978858298UL, 2811169155UL, 523464422UL, 226935048UL, 3040519789UL,
     3175145892UL, 100435649UL, 390670639UL, 2952089162UL, 841119475UL, 2325614998UL, 2553003640UL, 546822429UL,
     2029308235UL, 3225988654UL, 3539796416UL, 1782671013UL, 4153826844UL, 1328167289UL, 1570739863UL, 3844338162UL,
     1298864389UL, 4124540512UL, 3882013070UL, 1608431339UL, 3255406162UL, 2058742071UL, 1744848601UL, 3501990332UL,
     2296328682UL, 811816591UL, 584513889UL, 2590678532UL, 129869501UL, 3204563416UL, 2914283062UL, 352848211UL,
     494030490UL, 2781751807UL, 3078325777UL, 264757620UL, 2450577869UL, 715964072UL, 941166918UL, 2158327331UL,
     3636881013UL, 1618608400UL, 1926213374UL, 3396585883UL, 1470427426UL, 4011365959UL, 4255988137UL, 1158766284UL,
     1984818694UL, 3471935843UL, 3695453837UL, 1693991400UL, 4180638033UL, 1100160564UL, 1395044826UL, 3952793279UL,
     3019491049UL, 189112716UL, 435162722UL, 2706139399UL, 1016811966UL, 2217162459UL, 2526189877UL, 774831696UL,
     643086745UL, 2666061564UL, 2354934034UL, 887166583UL, 2838900430UL, 294275499UL, 54519365UL, 3145957664UL,
     3823145334UL, 1532818963UL, 1240029693UL, 4048895640UL, 1820460577UL, 3560857924UL, 3331051178UL, 2117577167UL,
     3598663992UL, 1858283101UL, 2088143283UL, 3301633750UL, 1495127663UL, 3785470218UL, 4078182116UL, 1269332353UL,
     332098007UL, 2876706482UL, 3116540252UL, 25085497UL, 2628386432UL, 605395429UL, 916469259UL, 2384220526UL,
     2254837415UL, 1054503362UL, 745528876UL, 2496903497UL, 151290352UL, 2981684885UL, 2735556987UL, 464596510UL,
     1137851976UL, 4218313005UL, 3923506883UL, 1365741990UL, 3434129695UL, 1946996346UL, 1723425172UL, 3724871409UL
};


/** This polynomial (0x04c11db7) is used at: AUTODIN II, Ethernet, & FDDI  */

unsigned  int crc32_table[]= {
     0x00000000UL, 0x04c11db7UL, 0x09823b6eUL, 0x0d4326d9UL,
     0x130476dcUL, 0x17c56b6bUL, 0x1a864db2UL, 0x1e475005UL,
     0x2608edb8UL, 0x22c9f00fUL, 0x2f8ad6d6UL, 0x2b4bcb61UL,
     0x350c9b64UL, 0x31cd86d3UL, 0x3c8ea00aUL, 0x384fbdbdUL,
     0x4c11db70UL, 0x48d0c6c7UL, 0x4593e01eUL, 0x4152fda9UL,
     0x5f15adacUL, 0x5bd4b01bUL, 0x569796c2UL, 0x52568b75UL,
     0x6a1936c8UL, 0x6ed82b7fUL, 0x639b0da6UL, 0x675a1011UL,
     0x791d4014UL, 0x7ddc5da3UL, 0x709f7b7aUL, 0x745e66cdUL,
     0x9823b6e0UL, 0x9ce2ab57UL, 0x91a18d8eUL, 0x95609039UL,
     0x8b27c03cUL, 0x8fe6dd8bUL, 0x82a5fb52UL, 0x8664e6e5UL,
     0xbe2b5b58UL, 0xbaea46efUL, 0xb7a96036UL, 0xb3687d81UL,
     0xad2f2d84UL, 0xa9ee3033UL, 0xa4ad16eaUL, 0xa06c0b5dUL,
     0xd4326d90UL, 0xd0f37027UL, 0xddb056feUL, 0xd9714b49UL,
     0xc7361b4cUL, 0xc3f706fbUL, 0xceb42022UL, 0xca753d95UL,
     0xf23a8028UL, 0xf6fb9d9fUL, 0xfbb8bb46UL, 0xff79a6f1UL,
     0xe13ef6f4UL, 0xe5ffeb43UL, 0xe8bccd9aUL, 0xec7dd02dUL,
     0x34867077UL, 0x30476dc0UL, 0x3d044b19UL, 0x39c556aeUL,
     0x278206abUL, 0x23431b1cUL, 0x2e003dc5UL, 0x2ac12072UL,
     0x128e9dcfUL, 0x164f8078UL, 0x1b0ca6a1UL, 0x1fcdbb16UL,
     0x018aeb13UL, 0x054bf6a4UL, 0x0808d07dUL, 0x0cc9cdcaUL,
     0x7897ab07UL, 0x7c56b6b0UL, 0x71159069UL, 0x75d48ddeUL,
     0x6b93dddbUL, 0x6f52c06cUL, 0x6211e6b5UL, 0x66d0fb02UL,
     0x5e9f46bfUL, 0x5a5e5b08UL, 0x571d7dd1UL, 0x53dc6066UL,
     0x4d9b3063UL, 0x495a2dd4UL, 0x44190b0dUL, 0x40d816baUL,
     0xaca5c697UL, 0xa864db20UL, 0xa527fdf9UL, 0xa1e6e04eUL,
     0xbfa1b04bUL, 0xbb60adfcUL, 0xb6238b25UL, 0xb2e29692UL,
     0x8aad2b2fUL, 0x8e6c3698UL, 0x832f1041UL, 0x87ee0df6UL,
     0x99a95df3UL, 0x9d684044UL, 0x902b669dUL, 0x94ea7b2aUL,
     0xe0b41de7UL, 0xe4750050UL, 0xe9362689UL, 0xedf73b3eUL,
     0xf3b06b3bUL, 0xf771768cUL, 0xfa325055UL, 0xfef34de2UL,
     0xc6bcf05fUL, 0xc27dede8UL, 0xcf3ecb31UL, 0xcbffd686UL,
     0xd5b88683UL, 0xd1799b34UL, 0xdc3abdedUL, 0xd8fba05aUL,
     0x690ce0eeUL, 0x6dcdfd59UL, 0x608edb80UL, 0x644fc637UL,
     0x7a089632UL, 0x7ec98b85UL, 0x738aad5cUL, 0x774bb0ebUL,
     0x4f040d56UL, 0x4bc510e1UL, 0x46863638UL, 0x42472b8fUL,
     0x5c007b8aUL, 0x58c1663dUL, 0x558240e4UL, 0x51435d53UL,
     0x251d3b9eUL, 0x21dc2629UL, 0x2c9f00f0UL, 0x285e1d47UL,
     0x36194d42UL, 0x32d850f5UL, 0x3f9b762cUL, 0x3b5a6b9bUL,
     0x0315d626UL, 0x07d4cb91UL, 0x0a97ed48UL, 0x0e56f0ffUL,
     0x1011a0faUL, 0x14d0bd4dUL, 0x19939b94UL, 0x1d528623UL,
     0xf12f560eUL, 0xf5ee4bb9UL, 0xf8ad6d60UL, 0xfc6c70d7UL,
     0xe22b20d2UL, 0xe6ea3d65UL, 0xeba91bbcUL, 0xef68060bUL,
     0xd727bbb6UL, 0xd3e6a601UL, 0xdea580d8UL, 0xda649d6fUL,
     0xc423cd6aUL, 0xc0e2d0ddUL, 0xcda1f604UL, 0xc960ebb3UL,
     0xbd3e8d7eUL, 0xb9ff90c9UL, 0xb4bcb610UL, 0xb07daba7UL,
     0xae3afba2UL, 0xaafbe615UL, 0xa7b8c0ccUL, 0xa379dd7bUL,
     0x9b3660c6UL, 0x9ff77d71UL, 0x92b45ba8UL, 0x9675461fUL,
     0x8832161aUL, 0x8cf30badUL, 0x81b02d74UL, 0x857130c3UL,
     0x5d8a9099UL, 0x594b8d2eUL, 0x5408abf7UL, 0x50c9b640UL,
     0x4e8ee645UL, 0x4a4ffbf2UL, 0x470cdd2bUL, 0x43cdc09cUL,
     0x7b827d21UL, 0x7f436096UL, 0x7200464fUL, 0x76c15bf8UL,
     0x68860bfdUL, 0x6c47164aUL, 0x61043093UL, 0x65c52d24UL,
     0x119b4be9UL, 0x155a565eUL, 0x18197087UL, 0x1cd86d30UL,
     0x029f3d35UL, 0x065e2082UL, 0x0b1d065bUL, 0x0fdc1becUL,
     0x3793a651UL, 0x3352bbe6UL, 0x3e119d3fUL, 0x3ad08088UL,
     0x2497d08dUL, 0x2056cd3aUL, 0x2d15ebe3UL, 0x29d4f654UL,
     0xc5a92679UL, 0xc1683bceUL, 0xcc2b1d17UL, 0xc8ea00a0UL,
     0xd6ad50a5UL, 0xd26c4d12UL, 0xdf2f6bcbUL, 0xdbee767cUL,
     0xe3a1cbc1UL, 0xe760d676UL, 0xea23f0afUL, 0xeee2ed18UL,
     0xf0a5bd1dUL, 0xf464a0aaUL, 0xf9278673UL, 0xfde69bc4UL,
     0x89b8fd09UL, 0x8d79e0beUL, 0x803ac667UL, 0x84fbdbd0UL,
     0x9abc8bd5UL, 0x9e7d9662UL, 0x933eb0bbUL, 0x97ffad0cUL,
     0xafb010b1UL, 0xab710d06UL, 0xa6322bdfUL, 0xa2f33668UL,
     0xbcb4666dUL, 0xb8757bdaUL, 0xb5365d03UL, 0xb1f740b4UL
};

static const unsigned int crc32_table_b[256] = {
     0x0UL, 0x77073096UL, 0xEE0E612CUL, 0x990951BAUL, 0x76DC419UL,
     0x706AF48FUL, 0xE963A535UL, 0x9E6495A3UL, 0xEDB8832UL, 0x79DCB8A4UL,
     0xE0D5E91EUL, 0x97D2D988UL, 0x9B64C2BUL, 0x7EB17CBDUL, 0xE7B82D07UL,
     0x90BF1D91UL, 0x1DB71064UL, 0x6AB020F2UL, 0xF3B97148UL, 0x84BE41DEUL,
     0x1ADAD47DUL, 0x6DDDE4EBUL, 0xF4D4B551UL, 0x83D385C7UL, 0x136C9856UL,
     0x646BA8C0UL, 0xFD62F97AUL, 0x8A65C9ECUL, 0x14015C4FUL, 0x63066CD9UL,
     0xFA0F3D63UL, 0x8D080DF5UL, 0x3B6E20C8UL, 0x4C69105EUL, 0xD56041E4UL,
     0xA2677172UL, 0x3C03E4D1UL, 0x4B04D447UL, 0xD20D85FDUL, 0xA50AB56BUL,
     0x35B5A8FAUL, 0x42B2986CUL, 0xDBBBC9D6UL, 0xACBCF940UL, 0x32D86CE3UL,
     0x45DF5C75UL, 0xDCD60DCFUL, 0xABD13D59UL, 0x26D930ACUL, 0x51DE003AUL,
     0xC8D75180UL, 0xBFD06116UL, 0x21B4F4B5UL, 0x56B3C423UL, 0xCFBA9599UL,
     0xB8BDA50FUL, 0x2802B89EUL, 0x5F058808UL, 0xC60CD9B2UL, 0xB10BE924UL,
     0x2F6F7C87UL, 0x58684C11UL, 0xC1611DABUL, 0xB6662D3DUL, 0x76DC4190UL,
     0x1DB7106UL, 0x98D220BCUL, 0xEFD5102AUL, 0x71B18589UL, 0x6B6B51FUL,
     0x9FBFE4A5UL, 0xE8B8D433UL, 0x7807C9A2UL, 0xF00F934UL, 0x9609A88EUL,
     0xE10E9818UL, 0x7F6A0DBBUL, 0x86D3D2DUL, 0x91646C97UL, 0xE6635C01UL,
     0x6B6B51F4UL, 0x1C6C6162UL, 0x856530D8UL, 0xF262004EUL, 0x6C0695EDUL,
     0x1B01A57BUL, 0x8208F4C1UL, 0xF50FC457UL, 0x65B0D9C6UL, 0x12B7E950UL,
     0x8BBEB8EAUL, 0xFCB9887CUL, 0x62DD1DDFUL, 0x15DA2D49UL, 0x8CD37CF3UL,
     0xFBD44C65UL, 0x4DB26158UL, 0x3AB551CEUL, 0xA3BC0074UL, 0xD4BB30E2UL,
     0x4ADFA541UL, 0x3DD895D7UL, 0xA4D1C46DUL, 0xD3D6F4FBUL, 0x4369E96AUL,
     0x346ED9FCUL, 0xAD678846UL, 0xDA60B8D0UL, 0x44042D73UL, 0x33031DE5UL,
     0xAA0A4C5FUL, 0xDD0D7CC9UL, 0x5005713CUL, 0x270241AAUL, 0xBE0B1010UL,
     0xC90C2086UL, 0x5768B525UL, 0x206F85B3UL, 0xB966D409UL, 0xCE61E49FUL,
     0x5EDEF90EUL, 0x29D9C998UL, 0xB0D09822UL, 0xC7D7A8B4UL, 0x59B33D17UL,
     0x2EB40D81UL, 0xB7BD5C3BUL, 0xC0BA6CADUL, 0xEDB88320UL, 0x9ABFB3B6UL,
     0x3B6E20CUL, 0x74B1D29AUL, 0xEAD54739UL, 0x9DD277AFUL, 0x4DB2615UL,
     0x73DC1683UL, 0xE3630B12UL, 0x94643B84UL, 0xD6D6A3EUL, 0x7A6A5AA8UL,
     0xE40ECF0BUL, 0x9309FF9DUL, 0xA00AE27UL, 0x7D079EB1UL, 0xF00F9344UL,
     0x8708A3D2UL, 0x1E01F268UL, 0x6906C2FEUL, 0xF762575DUL, 0x806567CBUL,
     0x196C3671UL, 0x6E6B06E7UL, 0xFED41B76UL, 0x89D32BE0UL, 0x10DA7A5AUL,
     0x67DD4ACCUL, 0xF9B9DF6FUL, 0x8EBEEFF9UL, 0x17B7BE43UL, 0x60B08ED5UL,
     0xD6D6A3E8UL, 0xA1D1937EUL, 0x38D8C2C4UL, 0x4FDFF252UL, 0xD1BB67F1UL,
     0xA6BC5767UL, 0x3FB506DDUL, 0x48B2364BUL, 0xD80D2BDAUL, 0xAF0A1B4CUL,
     0x36034AF6UL, 0x41047A60UL, 0xDF60EFC3UL, 0xA867DF55UL, 0x316E8EEFUL,
     0x4669BE79UL, 0xCB61B38CUL, 0xBC66831AUL, 0x256FD2A0UL, 0x5268E236UL,
     0xCC0C7795UL, 0xBB0B4703UL, 0x220216B9UL, 0x5505262FUL, 0xC5BA3BBEUL,
     0xB2BD0B28UL, 0x2BB45A92UL, 0x5CB36A04UL, 0xC2D7FFA7UL, 0xB5D0CF31UL,
     0x2CD99E8BUL, 0x5BDEAE1DUL, 0x9B64C2B0UL, 0xEC63F226UL, 0x756AA39CUL,
     0x26D930AUL, 0x9C0906A9UL, 0xEB0E363FUL, 0x72076785UL, 0x5005713UL,
     0x95BF4A82UL, 0xE2B87A14UL, 0x7BB12BAEUL, 0xCB61B38UL, 0x92D28E9BUL,
     0xE5D5BE0DUL, 0x7CDCEFB7UL, 0xBDBDF21UL, 0x86D3D2D4UL, 0xF1D4E242UL,
     0x68DDB3F8UL, 0x1FDA836EUL, 0x81BE16CDUL, 0xF6B9265BUL, 0x6FB077E1UL,
     0x18B74777UL, 0x88085AE6UL, 0xFF0F6A70UL, 0x66063BCAUL, 0x11010B5CUL,
     0x8F659EFFUL, 0xF862AE69UL, 0x616BFFD3UL, 0x166CCF45UL, 0xA00AE278UL,
     0xD70DD2EEUL, 0x4E048354UL, 0x3903B3C2UL, 0xA7672661UL, 0xD06016F7UL,
     0x4969474DUL, 0x3E6E77DBUL, 0xAED16A4AUL, 0xD9D65ADCUL, 0x40DF0B66UL,
     0x37D83BF0UL, 0xA9BCAE53UL, 0xDEBB9EC5UL, 0x47B2CF7FUL, 0x30B5FFE9UL,
     0xBDBDF21CUL, 0xCABAC28AUL, 0x53B39330UL, 0x24B4A3A6UL, 0xBAD03605UL,
     0xCDD70693UL, 0x54DE5729UL, 0x23D967BFUL, 0xB3667A2EUL, 0xC4614AB8UL,
     0x5D681B02UL, 0x2A6F2B94UL, 0xB40BBE37UL, 0xC30C8EA1UL, 0x5A05DF1BUL,
     0x2D02EF8DUL
};

void mhash_clear_crc32 ( unsigned int * crc );
void mhash_get_crc32 ( const unsigned int *crc, void *ret );
void mhash_crc32 ( unsigned int *crc, const void *given_buf, unsigned int len );
void mhash_crc32b ( unsigned int *crc, const void *given_buf, unsigned int len );


void mhash_clear_crc32 ( unsigned int *crc )
{
     *crc = 0xffffffff;
     /*
      * preload shift register, per CRC-32 spec
      */
}

void mhash_get_crc32 ( const unsigned int *crc, void *ret )
{
     unsigned int tmp;
     tmp = ~ ( *crc );
     /*
      * transmit complement, per CRC-32 spec
      */
#if defined(WORDS_BIGENDIAN)
     tmp = mutils_word32swap ( tmp );
#endif
     if ( ret != NULL ) {
          memcpy ( ret, &tmp, sizeof ( unsigned int ) );
     }
}

void mhash_crc32 ( unsigned int *crc, const void *given_buf, unsigned int len )
{
     const unsigned char *p;

     if ( ( crc == NULL ) || ( given_buf == NULL ) || ( len == 0 ) ) {
          return;
     }

     for ( p = ( unsigned char* ) given_buf; len > 0; ++p, --len ) {
          ( *crc ) = ( ( *crc ) << 8 ) ^ crc32_table[ ( ( *crc ) >> 24 ) ^ *p];
     }
}

void
mhash_crc32b ( unsigned int *crc, const void *given_buf, unsigned int len )
{
     const unsigned char *p;

     if ( ( crc == NULL ) || ( given_buf == NULL ) || ( len == 0 ) ) {
          return;
     }

     for ( p = ( unsigned char* ) given_buf; len > 0; ++p, --len ) {
          ( *crc ) = ( ( ( *crc ) >> 8 ) & 0x00FFFFFF ) ^ crc32_table_b[ ( *crc ^ *p ) & 0xff];
     }
}


void incremental_ip_checksum ( int & old_cksum, unsigned char *addr, int len )
{
     int sum = old_cksum;
     unsigned short answer = 0;
     unsigned short *w = ( unsigned short* ) addr;
     int nleft = len;
     /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
     while ( nleft > 1 ) {
          sum += *w++;
          nleft -= 2;
     }
     /* mop up an odd byte, if necessary */
     if ( nleft == 1 ) {
          * ( unsigned char * ) ( &answer ) = * ( unsigned char * ) w;
          sum += answer;
     }
     old_cksum = sum;
}

unsigned short close_incremental_ip_checksum ( int & old_cksum )
{
     int sum = old_cksum;
     unsigned short answer = 0;

     /* add back carry outs from top 16 bits to low 16 bits */
     sum = ( sum >> 16 ) + ( sum & 0xffff ); /* add hi 16 to low 16 */
     sum += ( sum >> 16 ); /* add carry */
     answer = ( ~sum ) & 0x0000FFFF; /* truncate to 16 bits */
     return ( answer );
}

unsigned short ip_checksum ( unsigned char *addr, int len )
{
     int sum = 0;
     unsigned short answer = 0;

     unsigned short *w = ( unsigned short* ) addr;
     int nleft = len;
     /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
     while ( nleft > 1 ) {
          sum += *w++;
          nleft -= 2;
     }
     /* mop up an odd byte, if necessary */
     if ( nleft == 1 ) {
          * ( unsigned char * ) ( &answer ) = * ( unsigned char * ) w;
          sum += answer;
     }

     /* add back carry outs from top 16 bits to low 16 bits */
     sum = ( sum >> 16 ) + ( sum & 0xffff ); /* add hi 16 to low 16 */
     sum += ( sum >> 16 ); /* add carry */
     answer = ( unsigned short ) ( ( ~sum ) & 0x0000FFFF ); /* truncate to 16 bits */
     return ( answer );

}


/*
LA PARTE SORGENTE DELL'HASH MOLTO VELOCE!
*/

//**************************************
// Basic Types
//**************************************
#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L   // C99
# include <stdint.h>
typedef uint8_t  BYTE;
typedef uint16_t U16;
typedef uint32_t U32;
typedef  int32_t S32;
typedef uint64_t U64;
#else
typedef unsigned char       BYTE;
typedef unsigned short      U16;
typedef unsigned int        U32;
typedef   signed int        S32;
typedef unsigned long long  U64;
#endif



#if defined(XXH_FORCE_NATIVE_FORMAT) && (XXH_FORCE_NATIVE_FORMAT==1)
// Force native format. The result will be endian dependant.
#  define XXH_BIG_ENDIAN 0
#elif defined (__GLIBC__)
#  include <endian.h>
#  if (__BYTE_ORDER == __BIG_ENDIAN)
#     define XXH_BIG_ENDIAN 1
#  endif
#elif (defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN) || defined(_BIG_ENDIAN)) && !(defined(__LITTLE_ENDIAN__) || defined(__LITTLE_ENDIAN) || defined(_LITTLE_ENDIAN))
#  define XXH_BIG_ENDIAN 1
#elif defined(__sparc) || defined(__sparc__) \
    || defined(__ppc__) || defined(_POWER) || defined(__powerpc__) || defined(_ARCH_PPC) || defined(__PPC__) || defined(__PPC) || defined(PPC) || defined(__powerpc__) || defined(__powerpc) || defined(powerpc) \
    || defined(__hpux)  || defined(__hppa) \
    || defined(_MIPSEB) || defined(__s390__)
#  define XXH_BIG_ENDIAN 1
#endif

#if !defined(XXH_BIG_ENDIAN)
// Little Endian assumed. PDP Endian and other very rare endian format are unsupported.
#  define XXH_BIG_ENDIAN 0
#endif

#define GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#if defined(_WIN32)
#  define XXH_rotl32(x,r) _rotl(x,r)
#else
#  define XXH_rotl32(x,r) ((x << r) | (x >> (32 - r)))
#endif

#if defined(_MSC_VER)     // Visual Studio
#  define XXH_swap32 _byteswap_ulong
#elif GCC_VERSION >= 403
#  define XXH_swap32 __builtin_bswap32
#else
static inline U32 XXH_swap32 ( U32 x )
{
     return ( ( x << 24 ) & 0xff000000 ) |
            ( ( x <<  8 ) & 0x00ff0000 ) |
            ( ( x >>  8 ) & 0x0000ff00 ) |
            ( ( x >> 24 ) & 0x000000ff );
}
#endif




//**************************************
// Constants
//**************************************
#define PRIME32_1   2654435761U
#define PRIME32_2   2246822519U
#define PRIME32_3   3266489917U
#define PRIME32_4    668265263U
#define PRIME32_5    374761393U


//**************************************
// Macros
//**************************************
#define XXH_LE32(p)  (XXH_BIG_ENDIAN ? XXH_swap32(*(U32*)(p)) : *(U32*)(p))

//****************************
// Simple Hash Functions
//****************************

U32 XXH32 ( const void* input, int len, U32 seed )
{
#if 0
     // Simple version, good for code maintenance, but unfortunately slow for small inputs
     void* state = XXH32_init ( seed );
     XXH32_update ( state, input, len );
     return XXH32_digest ( state );
#else

     const BYTE* p = ( const BYTE* ) input;
     const BYTE* const bEnd = p + len;
     U32 h32;

#ifdef XXH_ACCEPT_NULL_INPUT_POINTER
     if ( p==NULL ) {
          len=0;
          p= ( const BYTE* ) 16;
     }
#endif

     if ( len>=16 ) {
          const BYTE* const limit = bEnd - 16;
          U32 v1 = seed + PRIME32_1 + PRIME32_2;
          U32 v2 = seed + PRIME32_2;
          U32 v3 = seed + 0;
          U32 v4 = seed - PRIME32_1;

          do {
               v1 += XXH_LE32 ( p ) * PRIME32_2;
               v1 = XXH_rotl32 ( v1, 13 );
               v1 *= PRIME32_1;
               p+=4;
               v2 += XXH_LE32 ( p ) * PRIME32_2;
               v2 = XXH_rotl32 ( v2, 13 );
               v2 *= PRIME32_1;
               p+=4;
               v3 += XXH_LE32 ( p ) * PRIME32_2;
               v3 = XXH_rotl32 ( v3, 13 );
               v3 *= PRIME32_1;
               p+=4;
               v4 += XXH_LE32 ( p ) * PRIME32_2;
               v4 = XXH_rotl32 ( v4, 13 );
               v4 *= PRIME32_1;
               p+=4;
          } while ( p<=limit );

          h32 = XXH_rotl32 ( v1, 1 ) + XXH_rotl32 ( v2, 7 ) + XXH_rotl32 ( v3, 12 ) + XXH_rotl32 ( v4, 18 );
     } else {
          h32  = seed + PRIME32_5;
     }

     h32 += ( U32 ) len;

     while ( p<=bEnd-4 ) {
          h32 += XXH_LE32 ( p ) * PRIME32_3;
          h32 = XXH_rotl32 ( h32, 17 ) * PRIME32_4 ;
          p+=4;
     }

     while ( p<bEnd ) {
          h32 += ( *p ) * PRIME32_5;
          h32 = XXH_rotl32 ( h32, 11 ) * PRIME32_1 ;
          p++;
     }

     h32 ^= h32 >> 15;
     h32 *= PRIME32_2;
     h32 ^= h32 >> 13;
     h32 *= PRIME32_3;
     h32 ^= h32 >> 16;

     return h32;

#endif
}


//****************************
// Advanced Hash Functions
//****************************

struct XXH_state32_t {
     U32 seed;
     U32 v1;
     U32 v2;
     U32 v3;
     U32 v4;
     U64 total_len;
     char memory[16];
     int memsize;
};


int XXH32_sizeofState()
{
     return sizeof ( struct XXH_state32_t );
}


XXH_errorcode XXH32_resetState ( void* state_in, unsigned int seed )
{
     struct XXH_state32_t * state = ( struct XXH_state32_t * ) state_in;
     state->seed = seed;
     state->v1 = seed + PRIME32_1 + PRIME32_2;
     state->v2 = seed + PRIME32_2;
     state->v3 = seed + 0;
     state->v4 = seed - PRIME32_1;
     state->total_len = 0;
     state->memsize = 0;
     return OK;
}


void* XXH32_init ( U32 seed )
{
     struct XXH_state32_t * state = ( struct XXH_state32_t * ) malloc ( sizeof ( struct XXH_state32_t ) );
     XXH32_resetState ( state, seed );
     return ( void* ) state;
}


XXH_errorcode XXH32_update ( void* state_in, const void* input, int len )
{
     struct XXH_state32_t * state = ( struct XXH_state32_t * ) state_in;
     const BYTE* p = ( const BYTE* ) input;
     const BYTE* const bEnd = p + len;

#ifdef XXH_ACCEPT_NULL_INPUT_POINTER
     if ( input==NULL ) {
          return XXH_ERROR;
     }
#endif

     state->total_len += len;

     if ( state->memsize + len < 16 ) { // fill in tmp buffer
          memcpy ( state->memory + state->memsize, input, len );
          state->memsize +=  len;
          return OK;
     }

     if ( state->memsize ) { // some data left from previous update
          memcpy ( state->memory + state->memsize, input, 16-state->memsize );
          {
               const U32* p32 = ( const U32* ) state->memory;
               state->v1 += XXH_LE32 ( p32 ) * PRIME32_2;
               state->v1 = XXH_rotl32 ( state->v1, 13 );
               state->v1 *= PRIME32_1;
               p32++;
               state->v2 += XXH_LE32 ( p32 ) * PRIME32_2;
               state->v2 = XXH_rotl32 ( state->v2, 13 );
               state->v2 *= PRIME32_1;
               p32++;
               state->v3 += XXH_LE32 ( p32 ) * PRIME32_2;
               state->v3 = XXH_rotl32 ( state->v3, 13 );
               state->v3 *= PRIME32_1;
               p32++;
               state->v4 += XXH_LE32 ( p32 ) * PRIME32_2;
               state->v4 = XXH_rotl32 ( state->v4, 13 );
               state->v4 *= PRIME32_1;
               p32++;
          }
          p += 16-state->memsize;
          state->memsize = 0;
     }

     if ( p <= bEnd-16 ) {
          const BYTE* const limit = bEnd - 16;
          U32 v1 = state->v1;
          U32 v2 = state->v2;
          U32 v3 = state->v3;
          U32 v4 = state->v4;

          do {
               v1 += XXH_LE32 ( p ) * PRIME32_2;
               v1 = XXH_rotl32 ( v1, 13 );
               v1 *= PRIME32_1;
               p+=4;
               v2 += XXH_LE32 ( p ) * PRIME32_2;
               v2 = XXH_rotl32 ( v2, 13 );
               v2 *= PRIME32_1;
               p+=4;
               v3 += XXH_LE32 ( p ) * PRIME32_2;
               v3 = XXH_rotl32 ( v3, 13 );
               v3 *= PRIME32_1;
               p+=4;
               v4 += XXH_LE32 ( p ) * PRIME32_2;
               v4 = XXH_rotl32 ( v4, 13 );
               v4 *= PRIME32_1;
               p+=4;
          } while ( p<=limit );

          state->v1 = v1;
          state->v2 = v2;
          state->v3 = v3;
          state->v4 = v4;
     }

     if ( p < bEnd ) {
          memcpy ( state->memory, p, bEnd-p );
          state->memsize = ( int ) ( bEnd-p );
     }

     return OK;
}


U32 XXH32_intermediateDigest ( void* state_in )
{
     struct XXH_state32_t * state = ( struct XXH_state32_t * ) state_in;
     BYTE * p   = ( BYTE* ) state->memory;
     BYTE* bEnd = ( BYTE* ) state->memory + state->memsize;
     U32 h32;


     if ( state->total_len >= 16 ) {
          h32 = XXH_rotl32 ( state->v1, 1 ) + XXH_rotl32 ( state->v2, 7 ) + XXH_rotl32 ( state->v3, 12 ) + XXH_rotl32 ( state->v4, 18 );
     } else {
          h32  = state->seed + PRIME32_5;
     }

     h32 += ( U32 ) state->total_len;

     while ( p<=bEnd-4 ) {
          h32 += XXH_LE32 ( p ) * PRIME32_3;
          h32 = XXH_rotl32 ( h32, 17 ) * PRIME32_4;
          p+=4;
     }

     while ( p<bEnd ) {
          h32 += ( *p ) * PRIME32_5;
          h32 = XXH_rotl32 ( h32, 11 ) * PRIME32_1;
          p++;
     }

     h32 ^= h32 >> 15;
     h32 *= PRIME32_2;
     h32 ^= h32 >> 13;
     h32 *= PRIME32_3;
     h32 ^= h32 >> 16;

     return h32;
}


U32 XXH32_digest ( void* state_in )
{
     U32 h32 = XXH32_intermediateDigest ( state_in );

     free ( state_in );

     return h32;
}
