static void *store_big_endian_signed_samples (void *dst, int32_t *src, int bps, int count)
{
    unsigned char *dptr = dst;
    int32_t temp;

    switch (bps) {

        case 1:
            while (count--)
                *dptr++ = *src++;

            break;

        case 2:
            while (count--) {
                *dptr++ = (unsigned char) ((temp = *src++) >> 8);
                *dptr++ = (unsigned char) temp;
            }

            break;

        case 3:
            while (count--) {
                *dptr++ = (unsigned char) ((temp = *src++) >> 16);
                *dptr++ = (unsigned char) (temp >> 8);
                *dptr++ = (unsigned char) temp;
            }

            break;

        case 4:
            while (count--) {
                *dptr++ = (unsigned char) ((temp = *src++) >> 24);
                *dptr++ = (unsigned char) (temp >> 16);
                *dptr++ = (unsigned char) (temp >> 8);
                *dptr++ = (unsigned char) temp;
            }

            break;
    }

    return dptr;
}