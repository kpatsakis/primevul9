static uint32_t qxl_crc32(const uint8_t *p, unsigned len)
{
    /*
     * zlib xors the seed with 0xffffffff, and xors the result
     * again with 0xffffffff; Both are not done with linux's crc32,
     * which we want to be compatible with, so undo that.
     */
    return crc32(0xffffffff, p, len) ^ 0xffffffff;
}