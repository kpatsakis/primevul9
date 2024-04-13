hsv2rgb(UINT8 *out, const UINT8 *in, int xsize) {  // following colorsys.py

    int p, q, t;
    UINT8 up, uq, ut;
    int i, x;
    float f, fs;
    UINT8 h, s, v;

    for (x = 0; x < xsize; x++, in += 4) {
        h = in[0];
        s = in[1];
        v = in[2];

        if (s == 0) {
            *out++ = v;
            *out++ = v;
            *out++ = v;
        } else {
            i = floor((float)h * 6.0 / 255.0);      // 0 - 6
            f = (float)h * 6.0 / 255.0 - (float)i;  // 0-1 : remainder.
            fs = ((float)s) / 255.0;

            p = round((float)v * (1.0 - fs));
            q = round((float)v * (1.0 - fs * f));
            t = round((float)v * (1.0 - fs * (1.0 - f)));
            up = (UINT8)CLIP8(p);
            uq = (UINT8)CLIP8(q);
            ut = (UINT8)CLIP8(t);

            switch (i % 6) {
                case 0:
                    *out++ = v;
                    *out++ = ut;
                    *out++ = up;
                    break;
                case 1:
                    *out++ = uq;
                    *out++ = v;
                    *out++ = up;
                    break;
                case 2:
                    *out++ = up;
                    *out++ = v;
                    *out++ = ut;
                    break;
                case 3:
                    *out++ = up;
                    *out++ = uq;
                    *out++ = v;
                    break;
                case 4:
                    *out++ = ut;
                    *out++ = up;
                    *out++ = v;
                    break;
                case 5:
                    *out++ = v;
                    *out++ = up;
                    *out++ = uq;
                    break;
            }
        }
        *out++ = in[3];
    }
}