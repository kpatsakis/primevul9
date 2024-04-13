ZrtpQueue::ZrtpQueue(uint32 size, RTPApplication& app) :
        AVPQueue(size,app)
{
    init();
}