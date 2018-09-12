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
#define SSIZE ssize_t
#define SOCKET_COND_OK != 0
#define PSOCKET_CAST(x) (x)
#define SOCKET_CAST(x) (x)
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

struct udpimpl_data {
    sockaddr addr;
    socklen_t len;
};

bool udp::open(const char *hostname, std::uint16_t port, sock_type *sock, void **data) {
    char s_port[6] { 0 };
    addrinfo hints;
    addrinfo *lookup_res { nullptr };

    std::memset(&hints, 0, sizeof(addrinfo));
    std::snprintf(s_port, 6, "%d", port);

    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_socktype = SOCK_DGRAM;

    if(getaddrinfo(hostname, s_port, &hints, &lookup_res))
        return false;

    bool has_socket;
    if(*sock == INVALID_SOCKET_RESOURCE) {
        has_socket =
                (*PSOCKET_CAST(sock)
                         = socket(lookup_res->ai_family, lookup_res->ai_socktype,
                                  lookup_res->ai_protocol)) SOCKET_COND_OK;
    } else
        has_socket = true;

    if(has_socket) {
        if(*data == nullptr)
            *data = new udpimpl_data;

        memcpy(&static_cast<udpimpl_data*>(*data)->addr, lookup_res->ai_addr, sizeof(sockaddr));
        memcpy(&static_cast<udpimpl_data*>(*data)->len, &lookup_res->ai_addrlen, sizeof(socklen_t));
    }

    freeaddrinfo(lookup_res);
    return has_socket;
}

void udp::close(sock_type sock, void **data) {
#if defined(__linux__)
    ::close(sock);
#elif defined(_WIN32)
	closesocket(SOCKET_CAST(sock));
#endif

    if(*data) {
        delete static_cast<udpimpl_data *>(*data);
        *data = nullptr;
    }
}

streamsz udp::write(const char *wdata, streamsz datasize, sock_type sock, void *data) noexcept {
    udpimpl_data* dconn = static_cast<udpimpl_data*>(data);

    streamsz written_bytes = static_cast<streamsz>(
            sendto(SOCKET_CAST(sock), wdata, static_cast<std::size_t>(datasize), 0, &dconn->addr, dconn->len)
    );

    if(written_bytes < 0)
        written_bytes = 0;

    return written_bytes;
}

streamsz udp::read(char *buffer, std::size_t readlen, sock_type sock, void *) noexcept {
    sockaddr client;
    socklen_t client_len = sizeof(sockaddr);

    std::memset(&client, 0, client_len);
    std::memset(buffer, 0, readlen);

    ssize_type current = recvfrom(SOCKET_CAST(sock), buffer, readlen, 0, &client, &client_len);
    if(current < 0)
        current = 0;

    return static_cast<streamsz>(current);
}
