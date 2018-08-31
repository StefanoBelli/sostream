//
// Created by ssynx on 30/08/18.
//

#include "sostream.hpp"

#include <cstring>


#ifdef __linux__
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
using ssize_type = ssize_t;
#define SOCKET_CAST(xsock) (xsock)
#elif defined(_WIN32)
#include <WinSock2.h>
#include <WS2tcpip.h>
using ssize_type = SSIZE_T;
#define SOCKET_CAST(xsock) (reinterpret_cast<SOCKET>(xsock))
#endif

using sock_type = ssynx::socket_resource_type;
using namespace ssynx::prot;


bool tcp::open(const char *hostname, std::uint16_t port, sock_type *sock) noexcept {
    char s_port[6] { 0 };
    addrinfo hints;
    addrinfo *lookup_res { nullptr };

    std::memset(&hints, 0, sizeof(hints));

    std::snprintf(s_port, 6, "%d", port);

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_family = AF_UNSPEC;

    if(getaddrinfo(hostname, s_port, &hints, &lookup_res))
        return false;

	bool res =
#if defined(__linux__)
		(*sock = socket(lookup_res->ai_family, lookup_res->ai_socktype, lookup_res->ai_protocol)) >= 0
#elif defined(_WIN32)
		(*(SOCKET*) sock = socket(
			lookup_res->ai_family,
			lookup_res->ai_socktype,
			lookup_res->ai_protocol)) != INVALID_SOCKET
#endif
		&& !connect(SOCKET_CAST(*sock), lookup_res->ai_addr, lookup_res->ai_addrlen);

    freeaddrinfo(lookup_res);
    return res;
}

void tcp::close(sock_type sock) noexcept {
#if defined(_WIN32)
	CloseHandle(reinterpret_cast<HANDLE>(sock));
#elif defined(__linux__)
    ::close(sock);
#endif
}

std::streamsize tcp::write(const char *data, std::streamsize datasize, sock_type sock) noexcept {
	return static_cast<std::streamsize>(
		send(SOCKET_CAST(sock), data, static_cast<std::size_t>(datasize), 0)
		);
}

//ioctl calls without error checking
std::streamsize tcp::read_all(std::vector<char> &buffer, sock_type sock) {
    ssize_type read_data = 0;
    std::size_t available_data = 0;
	
#if defined(_WIN32)
	DWORD out;
#endif

    do {
#if defined(__linux__)
        ioctl(sock, FIONREAD, &available_data);
#elif defined(_WIN32)
		WSAIoctl(SOCKET_CAST(sock), FIONREAD, nullptr, 0,
			&available_data, sizeof(available_data), &out, nullptr,
			nullptr);
#endif
        buffer.resize(available_data + read_data);
        read_data += recv(SOCKET_CAST(sock), buffer.data(), available_data, 0);
    } while(available_data > 0);

    return static_cast<std::streamsize>(read_data);
}
