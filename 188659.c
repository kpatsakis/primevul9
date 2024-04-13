JBIG2Bitmap::JBIG2Bitmap(JBIG2Bitmap *bitmap) : JBIG2Segment(0)
{
    if (unlikely(bitmap == nullptr)) {
        error(errSyntaxError, -1, "NULL bitmap in JBIG2Bitmap");
        w = h = line = 0;
        data = nullptr;
        return;
    }

    w = bitmap->w;
    h = bitmap->h;
    line = bitmap->line;

    if (w <= 0 || h <= 0 || line <= 0 || h >= (INT_MAX - 1) / line) {
        error(errSyntaxError, -1, "invalid width/height");
        data = nullptr;
        return;
    }
    // need to allocate one extra guard byte for use in combine()
    data = (unsigned char *)gmalloc(h * line + 1);
    memcpy(data, bitmap->data, h * line);
    data[h * line] = 0;
}