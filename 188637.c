JBIG2Bitmap::JBIG2Bitmap(unsigned int segNumA, int wA, int hA) : JBIG2Segment(segNumA)
{
    w = wA;
    h = hA;
    int auxW;
    if (unlikely(checkedAdd(wA, 7, &auxW))) {
        error(errSyntaxError, -1, "invalid width");
        data = nullptr;
        return;
    }
    line = auxW >> 3;

    if (w <= 0 || h <= 0 || line <= 0 || h >= (INT_MAX - 1) / line) {
        error(errSyntaxError, -1, "invalid width/height");
        data = nullptr;
        return;
    }
    // need to allocate one extra guard byte for use in combine()
    data = (unsigned char *)gmalloc_checkoverflow(h * line + 1);
    if (data != nullptr) {
        data[h * line] = 0;
    }
}