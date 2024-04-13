rgbA2rgba(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    unsigned int alpha, tmp;
    for (x = 0; x < xsize; x++) {
        alpha = in[3];
        *out++ = MULDIV255(*in++, alpha, tmp);
        *out++ = MULDIV255(*in++, alpha, tmp);
        *out++ = MULDIV255(*in++, alpha, tmp);
        *out++ = *in++;
    }
}