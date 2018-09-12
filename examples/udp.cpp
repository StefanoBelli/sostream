#include <sostream.hpp>
#include <iostream>

using namespace ssynx;

int buf_size(std::iostream& ios) {
    ios.seekg(0, std::ios::end);
    int bufs = ios.tellg();
    ios.seekg(0, std::ios::beg);
    
    return bufs - 1;
}

int main() {
    ipc::socketstream<prot::udp> sock("127.0.0.1", 12345);

    std::string s;

    while(1) {
        std::getline(std::cin, s);

        sock << s;

        sock.peek();

        int available_data = buf_size(sock);
        std::cout << "there is available data! " << available_data << '\n';

        while(available_data--) {
            char s = sock.get();
            if(s == '\n')
                std::cout << "<newline>\n";
            else 
                std::cout << static_cast<char>(s);
        }
    }
    return 0;
}
