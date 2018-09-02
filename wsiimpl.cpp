//
// Windows Socket library initialization
// Initializes WinSock 2.2
//
//  NO ERROR CHECKING PERFORMED, ANY ERROR IGNORED
//

#include <WinSock2.h>
#include <sostream.hpp>

void _Internal_WSA_Init() {
	WSADATA __unused_sock_data;
	WSAStartup(MAKEWORD(2, 2), &__unused_sock_data);
}

void _Internal_WSA_Cleanup() {
	WSACleanup();
}