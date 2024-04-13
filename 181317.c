static inline uint8_t mirror8bit(uint8_t byte)
{
    return (byte * 0x0202020202ULL & 0x010884422010ULL) % 1023;
}