#include <iostream>
#include <sstream>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <string>

using namespace std;

std::map<string, string> GetIpAndInterfaces();
string GetIp ( int a_socket, bool local );
string GetRemotePeerMac ( std::map<string, string> & ip_ifname, int the_socket );

