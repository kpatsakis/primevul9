void JBIG2Bitmap::expand(int newH, unsigned int pixel)
{
    if (unlikely(!data)) {
        return;
    }
    if (newH <= h || line <= 0 || newH >= (INT_MAX - 1) / line) {
        error(errSyntaxError, -1, "invalid width/height");
        gfree(data);
        data = nullptr;
        return;
    }
    // need to allocate one extra guard byte for use in combine()
    data = (unsigned char *)grealloc(data, newH * line + 1);
    if (pixel) {
        memset(data + h * line, 0xff, (newH - h) * line);
    } else {
        memset(data + h * line, 0x00, (newH - h) * line);
    }
    h = newH;
    data[h * line] = 0;
}