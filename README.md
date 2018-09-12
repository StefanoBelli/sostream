# sostream
A *PoC* socket stream based on std::socketbuf and std::iostream

### Note
This approach is not reccomended for connectionless transport protocols (e.g. UDP), for example we can't get the source address for a datagram (we could but needs to be done by the implementor)

I would use this only for connection-oriented transport protocols. Or if you want, you can implement your own UDP class which provides a static method to retrieve the last datagram's source address info. But it is kind of bad looking.

Anyway you may consider having a connectionless socketstream interface, well suited for connectionless protocols.
