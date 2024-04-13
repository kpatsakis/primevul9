rgbT2rgba(UINT8 *out, int xsize, int r, int g, int b) {
#ifdef WORDS_BIGENDIAN
    UINT32 trns = ((r & 0xff) << 24) | ((g & 0xff) << 16) | ((b & 0xff) << 8) | 0xff;
    UINT32 repl = trns & 0xffffff00;
#else
    UINT32 trns = (0xff << 24) | ((b & 0xff) << 16) | ((g & 0xff) << 8) | (r & 0xff);
    UINT32 repl = trns & 0x00ffffff;
#endif

    int i;

    for (i = 0; i < xsize; i++, out += sizeof(trns)) {
        UINT32 v;
        memcpy(&v, out, sizeof(v));
        if (v == trns) {
            memcpy(out, &repl, sizeof(repl));
        }
    }
}