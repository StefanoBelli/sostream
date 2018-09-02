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

#endif

using namespace ssynx::prot;
using sock_type = ssynx::socket_resource_type;
using streamsz = std::streamsize;
using ssize_type = ssize_t;

bool tcp::open(const char *hostname, std::uint16_t port, ssynx::socket_resource_type *sock) noexcept {
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
            (*sock = socket(lookup_res->ai_family, lookup_res->ai_socktype, lookup_res->ai_protocol)) SOCKET_COND_OK
            && !connect(*sock, lookup_res->ai_addr, lookup_res->ai_addrlen);

    freeaddrinfo(lookup_res);
    return is_connected;
}

void tcp::close(ssynx::socket_resource_type sock) noexcept {
    ::close(sock);
}

streamsz tcp::write(const char *data, streamsz datasize, ssynx::socket_resource_type sock) noexcept {
    return static_cast<streamsz>(
            send(sock, data, static_cast<std::size_t>(datasize), 0)
            );
}

streamsz tcp::read(char* buffer, std::size_t readlen, ssynx::socket_resource_type sock) noexcept {
    std::memset(buffer, 0, readlen);
    ssize_type current = recv(sock, buffer, readlen, 0);

    if(current == -1)
        current = 0;

    return static_cast<streamsz>(current);
}

