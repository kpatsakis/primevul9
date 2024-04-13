static void *store_little_endian_unsigned_samples (void *dst, int32_t *src, int bps, int count)
{
    unsigned char *dptr = dst;
    int32_t temp;

    switch (bps) {

        case 1:
            while (count--)
                *dptr++ = *src++ + 0x80;

            break;

        case 2:
            while (count--) {
                *dptr++ = (unsigned char) (temp = *src++ + 0x8000);
                *dptr++ = (unsigned char) (temp >> 8);
            }

            break;

        case 3:
            while (count--) {
                *dptr++ = (unsigned char) (temp = *src++ + 0x800000);
                *dptr++ = (unsigned char) (temp >> 8);
                *dptr++ = (unsigned char) (temp >> 16);
            }

            break;

        case 4:
            while (count--) {
                *dptr++ = (unsigned char) (temp = *src++ + 0x80000000);
                *dptr++ = (unsigned char) (temp >> 8);
                *dptr++ = (unsigned char) (temp >> 16);
                *dptr++ = (unsigned char) (temp >> 24);
            }

            break;
    }

    return dptr;
}