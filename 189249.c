uint32 IncomingZRTPPkt::getSSRC() const {
     return ntohl(getHeader()->sources[0]);
}