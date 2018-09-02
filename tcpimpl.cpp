//
// Created by ssynx on 02/09/18.
//

#include <sostream.hpp>

#include <cstring>

#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#define SOCKET_COND_OK >= 0
#define SSIZE ssize_t
#define SOCKET_CAST(s) (s)
#define PSOCKET_CAST(s) (s)
#elif defined(_WIN32)
#include <Winsock2.h>
#include <Ws2tcpip.h>
#define SSIZE SSIZE_T
#define SOCKET_COND_OK != INVALID_SOCKET
#define SOCKET_CAST(s) (reinterpret_cast<SOCKET>(s))
#define PSOCKET_CAST(s) (reinterpret_cast<SOCKET*>(s))
#endif

using namespace ssynx::prot;
using sock_type = ssynx::socket_resource_type;
using streamsz = std::streamsize;
using ssize_type = SSIZE;

bool tcp::open(const char *hostname, std::uint16_t port, ssynx::socket_resource_type *sock, void**) noexcept {
    char s_port[6] { 0 };
    addrinfo hints;
    addrinfo *lookup_res { nullptr };

    std::memset(&hints, 0, sizeof(addrinfo));
    std::snprintf(s_port, 6, "%d", port);

    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_socktype = SOCK_STREAM;

    if(getaddrinfo(hostname, s_port, &hints, &lookup_res))
        return false;

    bool is_connected =
            (*PSOCKET_CAST(sock) 
				= socket(lookup_res->ai_family, lookup_res->ai_socktype, lookup_res->ai_protocol)) SOCKET_COND_OK
            && !connect(SOCKET_CAST(*sock), lookup_res->ai_addr, lookup_res->ai_addrlen);

    freeaddrinfo(lookup_res);
    return is_connected;
}

void tcp::close(ssynx::socket_resource_type sock, void**) noexcept {
#ifdef __linux__
    ::close(sock);
#elif defined(_WIN32)
	closesocket(SOCKET_CAST(sock));
#endif
}

streamsz tcp::write(const char *data, streamsz datasize, ssynx::socket_resource_type sock, void*) noexcept {
    streamsz written_bytes = static_cast<streamsz>(
            send(SOCKET_CAST(sock), data, static_cast<std::size_t>(datasize), 0)
            );

    if(written_bytes < 0)
        written_bytes = 0;

    return written_bytes;
}

streamsz tcp::read(char* buffer, std::size_t readlen, ssynx::socket_resource_type sock, void*) noexcept {
    std::memset(buffer, 0, readlen);
    ssize_type current = recv(SOCKET_CAST(sock), buffer, readlen, 0);

    if(current < 0)
        current = 0;

    return static_cast<streamsz>(current);
}
