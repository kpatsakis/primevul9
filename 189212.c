ZrtpQueue::ZrtpQueue(uint32 ssrc, uint32 size, RTPApplication& app) :
        AVPQueue(ssrc,size,app)
{
    init();
}