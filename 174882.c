la2lA(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    unsigned int alpha, pixel;
    for (x = 0; x < xsize; x++, in += 4) {
        alpha = in[3];
        if (alpha == 255 || alpha == 0) {
            pixel = in[0];
        } else {
            pixel = CLIP8((255 * in[0]) / alpha);
        }
        *out++ = (UINT8)pixel;
        *out++ = (UINT8)pixel;
        *out++ = (UINT8)pixel;
        *out++ = (UINT8)alpha;
    }
}