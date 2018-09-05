// $CXX $STD_CXX11 $INCLUDE_../include $BUILD_DIR/libsostream_impls.a

#include <sostream.hpp>
#include <iostream>

using namespace ssynx;

int main(int argc, const char** argv) {
    if(argc < 2) {
        std::cerr << "usage: " << argv[0] << " [url]\n";
        return 1;
    }

    ipc::socketstream<prot::tcp> s;
    
    std::cout << "opening connection with: " << argv[1] << ", port 80\n";
    if(!s.open(argv[1], 80)) {
        std::cerr << "could not open connection with " << argv[1] << "!\n";
        return 1;
    }
    
    std::cout << "sending HTTP GET request...\n";

    s << "GET /index.html HTTP/1.1\nHost: " << argv[1] << "\nUser-Agent: sostream_example\nConnection: close\r\n\r\n";

    std::cout << "waiting for response...\n\n";

    std::string each_line;
    while(!s.eof()) { //while connection is still up
        std::getline(s, each_line);
        std::cout << each_line << '\n';
    }

    return 0;
}
