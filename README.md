# sostream
A *PoC* socket stream based on std::streambuf and std::iostream

### Note
This approach is not reccomended for connectionless transport protocols (e.g. UDP), for example we can't get the source address for a received datagram (we could but needs to be done by the implementor)

I would use this only for connection-oriented transport protocols. Or if you want, you can implement your own UDP class which provides a static method to retrieve the last datagram's source address info. But it is kind of bad looking.

Anyway you may consider having a connectionless socketstream interface, well suited for connectionless protocols.

~~~
socketstream<udp> sock;
sock.open("127.0.0.1",12345);
sock << "test0";

sock.open("192.168.1.10",12345);
sock << "sending datagram to another host";

std::string formatted_dst;
char c;

//either by using one of these methods
sock >> formatted_dst;
std::getline(sock, formatted_dst);
sock.get();
sock.get(c);

sock.peek();
int len = gbuf_size(sock);
while(len--) {
    std::cout << static_cast<char>(sock.get());
}
// (or whatever way you want by using iostreams)

//... couldn't get addr info for the RECEIVED datagram
~~~

### The main problem with UDP

**WITH THE CONNECTION-ORIENTED INTERFACE**

Even if we were doing something like

~~~
prot::udp::last_datagram_address();
~~~

*(this static member function is not provided here)*

*(this static member function has to be provided from the implementor, not socketstream)*

we have to deal with synchronization if we're using multiple threads, because there is no way to do this, if not by using a global shared resource inside the implementation's translation unit.

#### udpimpl.cpp

~~~
//...

//WARNING!!
//WE MUST SYNCHRONIZE ACCESS TO THIS RESOURCE
//FROM MULTIPLE THREADS TO AVOID RACE CONDITIONS!!!
static datagram_address_info g_address_info;

//...

std::streamsize read(/*...*/) {
    //...
    //multiple threads may write at the same time
    recvfrom(/*...*/, /*...sockaddr*/g_address_info.address, /*...socklen_t*/g_address_info.len);
    //...
}

//...

datagram_address_info last_datagram_addrees() {
    // we want to avoid dirty reads...
    return g_address_info;
}
~~~

So you will agree with me that this is very bad looking and above all, not efficient if using multiple threads.

**WITH THE CONNECTIONLESS-ORIENTED INTERFACE**

*(function member is part of socketstream_connectionless itself, not the static member function of implementor)*

If we were using a connectionless interface, the problem of synchronization wouldn't exist but we would still use a function to retrieve the last address.

We can't do more, using iostreams

A simple socketstream_connectionless would look like a socketstream, except for a private member and the fact that the static method read() of the implementor requires to have a parameter lvalue-reference to a structure which is the private member of the class. Then it will be used by the public method last_datagram_address to retrieve the datagram source by the user.

~~~
//...
struct datagram_address_info {
    address;
    port;
};

struct udp {
    public:
    //...
    /*...*/ readfrom(/*...*/,datagram_address_info& addrinfo);
};

<ImplType>
class socketstream_connectionless {
    //...
public:
    int underflow() {
        //...
        ImplType::readfrom(/*...*/,last_dgram_info);
        //...
    }

    datagram_connection_info last_datagram_address() const noexcept {
        return last_dgram_addrinfo;
    }
private:
    //...
    datagram_address_info last_dgram_addrinfo;
};
//,,,
std::string fmt_data;

socketstream_connectionless<udp> sock;

sock.open("127.0.0.1",12345);
sock << "hello, i would like to get that\r\n\r\n";

//no synchronization method
sock >> fmt_data;
datagram_address_info addr = sock.last_datagram_address();

sock >> fmt_data;
datagram_address_info another_addr = sock.last_datagram_address();
~~~

### UDP socket notes

if using this UDP implementation, when we 

~~~
open()
~~~

a new socket gets created if none exist and an address is associated for

~~~
write()
~~~

to it.

If we call multiple times open() we can write to different addresses without closing every time the socket. The socket gets closed when you call

~~~
close()
~~~

So calling open without a previous close() will only change the write() address destination
