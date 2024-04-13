rgba2rgbA(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    unsigned int alpha;
    for (x = 0; x < xsize; x++, in += 4) {
        alpha = in[3];
        if (alpha == 255 || alpha == 0) {
            *out++ = in[0];
            *out++ = in[1];
            *out++ = in[2];
        } else {
            *out++ = CLIP8((255 * in[0]) / alpha);
            *out++ = CLIP8((255 * in[1]) / alpha);
            *out++ = CLIP8((255 * in[2]) / alpha);
        }
        *out++ = in[3];
    }
}