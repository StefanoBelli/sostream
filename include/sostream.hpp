#ifndef SOSTREAM_HPP
#define SOSTREAM_HPP

#include <streambuf>
#include <array>
#include <istream>
#include <cstdint>

#if defined(_WIN32)
void _Internal_WSA_Init();
void _Internal_WSA_Cleanup();
#define INVALID_SOCKET_RESOURCE nullptr
#define SOCKET_RES_TYPE void*
#elif defined(__linux__)
#define INVALID_SOCKET_RESOURCE -1
#define SOCKET_RES_TYPE int
#endif

namespace ssynx {

    using socket_resource_type = SOCKET_RES_TYPE;
    constexpr std::size_t SOCKET_READ_CHUNKSIZE = 4096;

    namespace prot {
        struct tcp {
        public:
            static bool open(const char *hostname,
                             std::uint16_t port, socket_resource_type *sock, void**) noexcept;

            static std::streamsize write(const char *data,
                                         std::streamsize datasize, socket_resource_type sock, void*) noexcept;

            static std::streamsize read(char* buffer, std::size_t readlen, socket_resource_type sock, void*) noexcept;

            static void close(socket_resource_type sock, void**) noexcept;
        };

        struct udp {
        public:
            static bool open(const char *hostname,
                             std::uint16_t port, socket_resource_type *sock, void**);

            static std::streamsize write(const char *data,
                                         std::streamsize datasize, socket_resource_type sock, void*) noexcept;

            static std::streamsize read(char* buffer, std::size_t readlen, socket_resource_type sock, void*) noexcept;

            static void close(socket_resource_type sock, void**);
        };

    }

    namespace ipc {

        template<typename ImplProvider,
                typename CharT,
                typename CharTraits = std::char_traits <CharT> >
        class basic_socketbuf : public std::basic_streambuf<CharT, CharTraits> {
        public:
            using char_type = CharT;
            using impl_type = ImplProvider;
            using int_type = typename CharTraits::int_type;
            using Base = std::basic_streambuf<CharT, CharTraits>;

            basic_socketbuf() {
#if defined(_WIN32)
                _Internal_WSA_Init();
#endif
                Base::setg(get_buffer_beginning, get_buffer_beginning + 1, get_buffer_beginning + 1);
            }

#if defined(_WIN32)
            ~basic_socketbuf() {
                _Internal_WSA_Cleanup();
            }
#else

            ~basic_socketbuf() = default;

#endif

            inline bool open(const char *hostn, std::uint16_t port) {
                return impl_type::open(hostn, port, &socket_res, &additional_data);
            }

            inline void close() {
                impl_type::close(socket_res, &additional_data);
                socket_res = INVALID_SOCKET_RESOURCE;
            }

            std::streamsize xsputn(char_type const *data, std::streamsize size) override {
                return impl_type::write(data, size, socket_res, additional_data);
            }

            int_type underflow() override {
                std::fill(get_buffer.begin(), get_buffer.end(), 0);

                std::streamsize read_size =
                        impl_type::read(get_buffer_beginning, SOCKET_READ_CHUNKSIZE, socket_res, additional_data);

                if(read_size == 0)
                    return EOF;

                Base::setg(get_buffer_beginning, get_buffer_beginning, get_buffer_beginning + read_size + 1);

                return CharTraits::to_int_type(get_buffer[0]);
            }

            inline socket_resource_type system_socket_resource() const noexcept {
                return socket_res;
            }

            //having off different than zero will result in no effect
            //if dir is std::ios_base::end
            std::streampos seekoff(std::streamoff off, std::ios_base::seekdir dir,
                    std::ios_base::openmode which = std::ios_base::in) override {

                if(which == std::ios_base::out)
                    return -1;

                if (dir == std::ios_base::cur)
                    Base::gbump(off);
                else if (dir == std::ios_base::end)
                    Base::setg(Base::eback(), Base::egptr(), Base::egptr());
                else if (dir == std::ios_base::beg)
                    Base::setg(Base::eback(), Base::eback() + off, Base::egptr());

                return Base::gptr() - Base::eback();
            }
            
        private:
            std::array <char_type, SOCKET_READ_CHUNKSIZE> get_buffer {};
            char_type *get_buffer_beginning { get_buffer.data() };
            socket_resource_type socket_res { INVALID_SOCKET_RESOURCE };
            void* additional_data { nullptr };
        };

        template<typename ImplProvider,
                typename CharT,
                typename CharTraits = std::char_traits <CharT> >
        class basic_socketstream : public std::basic_iostream<CharT, CharTraits> {
        public:
            using char_type = CharT;
            using impl_type = ImplProvider;
            using Base = std::basic_iostream<CharT, CharTraits>;
            using BasicStreambuf = std::basic_streambuf<CharT, CharTraits>;

            basic_socketstream() :
                    Base(static_cast<BasicStreambuf *>(&underlying_buffer)) {
                Base::setstate(std::ios::failbit);
            }

            basic_socketstream(const char *hostn, std::uint16_t port) :
                    Base(static_cast<BasicStreambuf *>(&underlying_buffer)) {
                Base::setstate(std::ios::failbit);
                open(hostn, port);
            }

            ~basic_socketstream() {
                close();
            }

            bool open(const char *hostn, std::uint16_t port) {
                bool rv = underlying_buffer.open(hostn, port);

                if(rv)
                    Base::clear();

                return rv;
            }

            void close() {
                underlying_buffer.close();
                Base::setstate(std::ios::failbit);
            }

        private:
            basic_socketbuf<ImplProvider, CharT, CharTraits> underlying_buffer;
        };

        template<typename Impl>
        using socketbuf = basic_socketbuf<Impl, char>;

        template<typename Impl>
        using socketstream = basic_socketstream<Impl, char>;

    }

}

#endif //SOSTREAM_HPP
