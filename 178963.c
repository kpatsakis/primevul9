static inline uint64_t ati_reg_read_offs(uint32_t reg, int offs,
                                         unsigned int size)
{
    if (offs == 0 && size == 4) {
        return reg;
    } else {
        return extract32(reg, offs * BITS_PER_BYTE, size * BITS_PER_BYTE);
    }
}