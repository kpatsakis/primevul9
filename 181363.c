static inline uint64_t mirror64bit(uint64_t value)
{
    return ((uint64_t)mirror8bit(value         & 0xff) << 56) |
           ((uint64_t)mirror8bit((value >> 8)  & 0xff) << 48) |
           ((uint64_t)mirror8bit((value >> 16) & 0xff) << 40) |
           ((uint64_t)mirror8bit((value >> 24) & 0xff) << 32) |
           ((uint64_t)mirror8bit((value >> 32) & 0xff) << 24) |
           ((uint64_t)mirror8bit((value >> 40) & 0xff) << 16) |
           ((uint64_t)mirror8bit((value >> 48) & 0xff) << 8)  |
           ((uint64_t)mirror8bit((value >> 56) & 0xff));
}