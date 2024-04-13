static inline int ube16_to_cpu(const uint8_t *buf)
{
    return (buf[0] << 8) | buf[1];
}