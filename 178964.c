static inline void ati_reg_write_offs(uint32_t *reg, int offs,
                                      uint64_t data, unsigned int size)
{
    if (offs == 0 && size == 4) {
        *reg = data;
    } else {
        *reg = deposit32(*reg, offs * BITS_PER_BYTE, size * BITS_PER_BYTE,
                         data);
    }
}