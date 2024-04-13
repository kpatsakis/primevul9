rgb2hsv_row(UINT8 *out, const UINT8 *in) {  // following colorsys.py
    float h, s, rc, gc, bc, cr;
    UINT8 maxc, minc;
    UINT8 r, g, b;
    UINT8 uh, us, uv;

    r = in[0];
    g = in[1];
    b = in[2];
    maxc = MAX(r, MAX(g, b));
    minc = MIN(r, MIN(g, b));
    uv = maxc;
    if (minc == maxc) {
        uh = 0;
        us = 0;
    } else {
        cr = (float)(maxc - minc);
        s = cr / (float)maxc;
        rc = ((float)(maxc - r)) / cr;
        gc = ((float)(maxc - g)) / cr;
        bc = ((float)(maxc - b)) / cr;
        if (r == maxc) {
            h = bc - gc;
        } else if (g == maxc) {
            h = 2.0 + rc - bc;
        } else {
            h = 4.0 + gc - rc;
        }
        // incorrect hue happens if h/6 is negative.
        h = fmod((h / 6.0 + 1.0), 1.0);

        uh = (UINT8)CLIP8((int)(h * 255.0));
        us = (UINT8)CLIP8((int)(s * 255.0));
    }
    out[0] = uh;
    out[1] = us;
    out[2] = uv;
}