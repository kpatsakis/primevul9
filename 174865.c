cmyk2hsv(UINT8 *out, const UINT8 *in, int xsize) {
    int x, nk, tmp;
    for (x = 0; x < xsize; x++) {
        nk = 255 - in[3];
        out[0] = CLIP8(nk - MULDIV255(in[0], nk, tmp));
        out[1] = CLIP8(nk - MULDIV255(in[1], nk, tmp));
        out[2] = CLIP8(nk - MULDIV255(in[2], nk, tmp));
        rgb2hsv_row(out, out);
        out[3] = 255;
        out += 4;
        in += 4;
    }
}