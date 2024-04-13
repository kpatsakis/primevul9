ZrtpQueue::sendImmediate(uint32 stamp, const unsigned char* data, size_t len)
{
    OutgoingDataQueue::sendImmediate(stamp, data, len);
}