uint32 IncomingZRTPPkt::getZrtpMagic() const {
     return ntohl(getHeader()->timestamp);
}