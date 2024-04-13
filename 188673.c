    void setBitmap(unsigned int idx, JBIG2Bitmap *bitmap)
    {
        if (likely(idx < size)) {
            bitmaps[idx] = bitmap;
        }
    }