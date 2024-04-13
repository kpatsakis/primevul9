JBIG2SymbolDict::JBIG2SymbolDict(unsigned int segNumA, unsigned int sizeA) : JBIG2Segment(segNumA)
{
    ok = true;
    size = sizeA;
    if (size != 0) {
        bitmaps = (JBIG2Bitmap **)gmallocn_checkoverflow(size, sizeof(JBIG2Bitmap *));
        if (!bitmaps) {
            ok = false;
            size = 0;
        }
    } else {
        bitmaps = nullptr;
    }
    for (unsigned int i = 0; i < size; ++i) {
        bitmaps[i] = nullptr;
    }
    genericRegionStats = nullptr;
    refinementRegionStats = nullptr;
}