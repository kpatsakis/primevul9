j2ku_srgba_rgba(
    opj_image_t *in,
    const JPEG2KTILEINFO *tileinfo,
    const UINT8 *tiledata,
    Imaging im) {
    unsigned x0 = tileinfo->x0 - in->x0, y0 = tileinfo->y0 - in->y0;
    unsigned w = tileinfo->x1 - tileinfo->x0;
    unsigned h = tileinfo->y1 - tileinfo->y0;

    int shifts[4], offsets[4], csiz[4];
    const UINT8 *cdata[4];
    const UINT8 *cptr = tiledata;
    unsigned n, x, y;

    for (n = 0; n < 4; ++n) {
        cdata[n] = cptr;
        shifts[n] = 8 - in->comps[n].prec;
        offsets[n] = in->comps[n].sgnd ? 1 << (in->comps[n].prec - 1) : 0;
        csiz[n] = (in->comps[n].prec + 7) >> 3;

        if (csiz[n] == 3) {
            csiz[n] = 4;
        }

        if (shifts[n] < 0) {
            offsets[n] += 1 << (-shifts[n] - 1);
        }

        cptr += csiz[n] * w * h;
    }

    for (y = 0; y < h; ++y) {
        const UINT8 *data[4];
        UINT8 *row = (UINT8 *)im->image[y0 + y] + x0 * 4;
        for (n = 0; n < 4; ++n) {
            data[n] = &cdata[n][csiz[n] * y * w];
        }

        for (x = 0; x < w; ++x) {
            for (n = 0; n < 4; ++n) {
                UINT32 word = 0;

                switch (csiz[n]) {
                    case 1:
                        word = *data[n]++;
                        break;
                    case 2:
                        word = *(const UINT16 *)data[n];
                        data[n] += 2;
                        break;
                    case 4:
                        word = *(const UINT32 *)data[n];
                        data[n] += 4;
                        break;
                }

                row[n] = j2ku_shift(offsets[n] + word, shifts[n]);
            }
            row += 4;
        }
    }
}