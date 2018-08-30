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

    if(getaddrinfo(hostname, s_port, &hints, &lookup_res) < 0)
        return false;

    bool res =
            (*sock = socket(lookup_res->ai_family, lookup_res->ai_socktype, lookup_res->ai_protocol)) >= 0 &&
                    !connect(*sock, lookup_res->ai_addr, lookup_res->ai_addrlen);

    freeaddrinfo(lookup_res);
    return res;
}

void tcp::close(sock_type sock) noexcept {
    ::close(sock);
}

std::streamsize tcp::write(const char *data, std::streamsize datasize, sock_type sock) noexcept {
    return static_cast<std::streamsize>(
            send(sock, data, static_cast<std::size_t>(datasize), 0));
}

std::streamsize tcp::read_all(std::vector<char> &buffer, sock_type sock) {
    ssize_t read_data = 0;
    std::size_t available_data = 0;

    do {
        ioctl(sock, FIONREAD, &available_data); //no error checking for ioctl calls
        buffer.resize(available_data + read_data);
        read_data += recv(sock, buffer.data(), available_data, 0);
    } while(available_data > 0);

    return static_cast<std::streamsize>(read_data);
}
