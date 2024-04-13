j2ku_graya_la(
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
    int ashift = 8 - in->comps[1].prec;
    int aoffset = in->comps[1].sgnd ? 1 << (in->comps[1].prec - 1) : 0;
    int acsiz = (in->comps[1].prec + 7) >> 3;
    const UINT8 *atiledata;

    unsigned x, y;

    if (csiz == 3) {
        csiz = 4;
    }
    if (acsiz == 3) {
        acsiz = 4;
    }

    if (shift < 0) {
        offset += 1 << (-shift - 1);
    }
    if (ashift < 0) {
        aoffset += 1 << (-ashift - 1);
    }

    atiledata = tiledata + csiz * w * h;

    for (y = 0; y < h; ++y) {
        const UINT8 *data = &tiledata[csiz * y * w];
        const UINT8 *adata = &atiledata[acsiz * y * w];
        UINT8 *row = (UINT8 *)im->image[y0 + y] + x0 * 4;
        for (x = 0; x < w; ++x) {
            UINT32 word = 0, aword = 0, byte;

            switch (csiz) {
                case 1:
                    word = *data++;
                    break;
                case 2:
                    word = *(const UINT16 *)data;
                    data += 2;
                    break;
                case 4:
                    word = *(const UINT32 *)data;
                    data += 4;
                    break;
            }

            switch (acsiz) {
                case 1:
                    aword = *adata++;
                    break;
                case 2:
                    aword = *(const UINT16 *)adata;
                    adata += 2;
                    break;
                case 4:
                    aword = *(const UINT32 *)adata;
                    adata += 4;
                    break;
            }

            byte = j2ku_shift(offset + word, shift);
            row[0] = row[1] = row[2] = byte;
            row[3] = j2ku_shift(aoffset + aword, ashift);
            row += 4;
        }
    }
}