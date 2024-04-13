JBIG2SymbolDict::~JBIG2SymbolDict()
{
    unsigned int i;

    for (i = 0; i < size; ++i) {
        delete bitmaps[i];
    }
    gfree(bitmaps);
    if (genericRegionStats) {
        delete genericRegionStats;
    }
    if (refinementRegionStats) {
        delete refinementRegionStats;
    }
}