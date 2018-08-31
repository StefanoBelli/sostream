# sostream

Generic socket interface for C++

Not sure if 
```c++
int_type basic_socketbuf<ChT, ChTr>::underflow();
```
has to be implemented that way.


### Usage

```c++
#include <sostream.hpp>

using namespace ssynx;
using namespace ipc;

int main() {
    std::string one_single_word;

    socketstream<prot::tcp> tcp("google.com", 80);
    tcp << "GET...";
    tcp >> one_single_word;

    //do your stuff as you would with cout and cin

    return 0;
}
```

### ImplementationProvider

Implementation must provide the following functions:

 * ```c++ bool open(const char* hostname, std::uint16_t port, socket_resource_type* __out_sock) ```

    Function shall return true if a connection was correctly made, false otherwise. 
    
    If connection was not made, the value assigned to the memory pointed by __out_sock is ignored anyway.

 * ```c++ void close(socket_resource_type sock)```

    Function will close and should not perform error check. Ignored.

 * ```c++ std::streamsize write(const char* data, std::streamsize count, socket_resource_type sock)```

    Function shall return the number of actual written bytes.

 * ```c++ std::streamsize read_all(std::vector<char>& v, socket_resource_type sock)```

    Function shall return the number of bytes read and if connection was closed, then just 0.
    
Each of these function should not raise any exception (use ```c++ noexcept``` specifier)

### Aliases

Defined in ```c++ namespace ssynx::ipc```

```c++
template<typename ImplProvider, typename CharT, typename CharTraits = std::char_traits<CharT> >
    class basic_socketbuf : public std::basic_streambuf<CharT, CharTraits>;

template<typename ImplProvider, typename CharT, typename CharTraits = std::char_traits<CharT> >
    class basic_socketstream : public std::basic_iostream<CharT, CharTraits>;
```

Aliases are: 

```c++
socketbuf<Impl> = basic_socketbuf<Impl, char>;
socketstream<Impl> = basic_socketstream<Impl, char>;
```

with (for both):

```c++
CharT = char
CharTraits = std::char_traits<CharT> = std::char_traits<char>
```

### Note for Windows

Windows socket library (Winsock) requires explicit initialization and cleanup.

User must provide definition for the following function

```c++
void _Internal_WSA_Init();
void _Internal_WSA_Cleanup();
```

**DEFINED IN THE GLOBAL NAMESPACE**

Also note that these functions will be called on socketbuf object construction and destruction (not on open or close)

### Already defined implementor declaration

```c++
namespace ssynx {
    namespace prot {
        struct tcp;
        struct udp;
        struct ssl;
    }
}
```
