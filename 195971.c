uint32_t net_crc32_le(const uint8_t *p, int len)
{
    uint32_t crc;
    int carry, i, j;
    uint8_t b;

    crc = 0xffffffff;
    for (i = 0; i < len; i++) {
        b = *p++;
        for (j = 0; j < 8; j++) {
            carry = (crc & 0x1) ^ (b & 0x01);
            crc >>= 1;
            b >>= 1;
            if (carry) {
                crc ^= POLYNOMIAL_LE;
            }
        }
    }

    return crc;
}