JBIG2Bitmap *JBIG2Bitmap::getSlice(unsigned int x, unsigned int y, unsigned int wA, unsigned int hA)
{
    JBIG2Bitmap *slice;
    unsigned int xx, yy;

    if (!data) {
        return nullptr;
    }

    slice = new JBIG2Bitmap(0, wA, hA);
    if (slice->isOk()) {
        slice->clearToZero();
        for (yy = 0; yy < hA; ++yy) {
            for (xx = 0; xx < wA; ++xx) {
                if (getPixel(x + xx, y + yy)) {
                    slice->setPixel(xx, yy);
                }
            }
        }
    } else {
        delete slice;
        slice = nullptr;
    }
    return slice;
}