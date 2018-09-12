# sostream
A *PoC* socket stream based on std::socketbuf and std::iostream

### Note
This approach is not reccomended for connectionless transport protocols (e.g. UDP), for example we can't get the source address for a datagram (we could but needs to be done by the implementor)

I would use this only for connection-oriented transport protocols. Or if you want, you can implement your own UDP class which provides a static method to retrieve the last datagram's source address info. But it is kind of bad looking.

Anyway you may consider having a connectionless socketstream interface, well suited for connectionless protocols.

### The main problem with UDP connections

Even if we were doing something like

~~~
prot::udp::last_datagram_address();
~~~

we have to deal with synchronization if we're using threads, because there is no way to do this, if not by using a global shared resource inside the implementation's translation unit.

**WITH THE CONNECTION-ORIENTED INTERFACE**

If we were using a connectionless interface, the problem of synchronization wouldn't exist but we would still use a function to retrieve the last address.

We can't do more, using iostreams

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
