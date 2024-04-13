rgb2hsv(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in += 4, out += 4) {
        rgb2hsv_row(out, in);
        out[3] = in[3];
    }
}