j2ku_gray_rgb(
    opj_image_t *in,
    const JPEG2KTILEINFO *tileinfo,
    const UINT8 *tiledata,
    Imaging im) {
    unsigned x0 = tileinfo->x0 - in->x0, y0 = tileinfo->y0 - in->y0;
    unsigned w = tileinfo->x1 - tileinfo->x0;
    unsigned h = tileinfo->y1 - tileinfo->y0;

    int shift = 8 - in->comps[0].prec;
    int offset = in->comps[0].sgnd ? 1 << (in->comps[0].prec - 1) : 0;
    int csiz = (in->comps[0].prec + 7) >> 3;

    unsigned x, y;

    if (shift < 0) {
        offset += 1 << (-shift - 1);
    }

    if (csiz == 3) {
        csiz = 4;
    }

    switch (csiz) {
        case 1:
            for (y = 0; y < h; ++y) {
                const UINT8 *data = &tiledata[y * w];
                UINT8 *row = (UINT8 *)im->image[y0 + y] + x0;
                for (x = 0; x < w; ++x) {
                    UINT8 byte = j2ku_shift(offset + *data++, shift);
                    row[0] = row[1] = row[2] = byte;
                    row[3] = 0xff;
                    row += 4;
                }
            }
            break;
        case 2:
            for (y = 0; y < h; ++y) {
                const UINT16 *data = (UINT16 *)&tiledata[2 * y * w];
                UINT8 *row = (UINT8 *)im->image[y0 + y] + x0;
                for (x = 0; x < w; ++x) {
                    UINT8 byte = j2ku_shift(offset + *data++, shift);
                    row[0] = row[1] = row[2] = byte;
                    row[3] = 0xff;
                    row += 4;
                }
            }
            break;
        case 4:
            for (y = 0; y < h; ++y) {
                const UINT32 *data = (UINT32 *)&tiledata[4 * y * w];
                UINT8 *row = (UINT8 *)im->image[y0 + y] + x0;
                for (x = 0; x < w; ++x) {
                    UINT8 byte = j2ku_shift(offset + *data++, shift);
                    row[0] = row[1] = row[2] = byte;
                    row[3] = 0xff;
                    row += 4;
                }
            }
            break;
    }
}