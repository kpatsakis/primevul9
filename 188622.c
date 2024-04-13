void JBIG2Stream::resetGenericStats(unsigned int templ, JArithmeticDecoderStats *prevStats)
{
    int size;

    size = contextSize[templ];
    if (prevStats && prevStats->getContextSize() == size) {
        if (genericRegionStats->getContextSize() == size) {
            genericRegionStats->copyFrom(prevStats);
        } else {
            delete genericRegionStats;
            genericRegionStats = prevStats->copy();
        }
    } else {
        if (genericRegionStats->getContextSize() == size) {
            genericRegionStats->reset();
        } else {
            delete genericRegionStats;
            genericRegionStats = new JArithmeticDecoderStats(1 << size);
        }
    }
}