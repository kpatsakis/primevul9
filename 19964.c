static inline uint64_t memory_region_shift_write_access(uint64_t *value,
                                                        signed shift,
                                                        uint64_t mask)
{
    uint64_t tmp;

    if (shift >= 0) {
        tmp = (*value >> shift) & mask;
    } else {
        tmp = (*value << -shift) & mask;
    }

    return tmp;
}