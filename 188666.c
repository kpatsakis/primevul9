inline int JBIG2Bitmap::nextPixel(JBIG2BitmapPtr *ptr)
{
    int pix;

    if (!ptr->p) {
        pix = 0;
    } else if (ptr->x < 0) {
        ++ptr->x;
        pix = 0;
    } else {
        pix = (*ptr->p >> ptr->shift) & 1;
        if (++ptr->x == w) {
            ptr->p = nullptr;
        } else if (ptr->shift == 0) {
            ++ptr->p;
            ptr->shift = 7;
        } else {
            --ptr->shift;
        }
    }
    return pix;
}