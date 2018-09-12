#include <sostream.hpp>

using namespace ssynx;

int main() {
    {
        ipc::socketstream<prot::udp> sock;
    }

    {
        ipc::socketstream<prot::udp> sock;
        sock.open("127.0.0.1",12345);
    }
     
    {
        ipc::socketstream<prot::udp> sock;
        sock.close();
    }

    {
        ipc::socketstream<prot::udp> sock;
        sock.open("127.0.0.1",12345);
        sock.close();
    }

    {
        ipc::socketstream<prot::udp> sock("127.0.0.1",12345);
    }
     
    {
        ipc::socketstream<prot::udp> sock("127.0.0.1", 12345);
        sock.close();
    }

    {
        ipc::socketstream<prot::udp> sock("127.0.0.1", 12345);
        sock.close();
        sock.open("127.0.0.1",12345);
        sock.close();
        sock.open("127.0.0.1",12345);
    }
    
    return 0;
}
