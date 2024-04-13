static inline void cpu_to_ube16(uint8_t *buf, int val)
{
    buf[0] = val >> 8;
    buf[1] = val & 0xff;
}