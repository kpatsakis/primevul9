inline void JBIG2Bitmap::getPixelPtr(int x, int y, JBIG2BitmapPtr *ptr)
{
    if (y < 0 || y >= h || x >= w) {
        ptr->p = nullptr;
        ptr->shift = 0; // make gcc happy
        ptr->x = 0; // make gcc happy
    } else if (x < 0) {
        ptr->p = &data[y * line];
        ptr->shift = 7;
        ptr->x = x;
    } else {
        ptr->p = &data[y * line + (x >> 3)];
        ptr->shift = 7 - (x & 7);
        ptr->x = x;
    }
}