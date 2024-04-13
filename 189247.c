OutgoingZRTPPkt::OutgoingZRTPPkt(
    const unsigned char* const hdrext, uint32 hdrextlen) :
        OutgoingRTPPkt(NULL, 0, hdrext, hdrextlen, NULL ,0, 0, NULL)
{
    getHeader()->version = 0;
    getHeader()->timestamp = htonl(ZRTP_MAGIC);
}