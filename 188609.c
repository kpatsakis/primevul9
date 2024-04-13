void JBIG2Stream::resetRefinementStats(unsigned int templ, JArithmeticDecoderStats *prevStats)
{
    int size;

    size = refContextSize[templ];
    if (prevStats && prevStats->getContextSize() == size) {
        if (refinementRegionStats->getContextSize() == size) {
            refinementRegionStats->copyFrom(prevStats);
        } else {
            delete refinementRegionStats;
            refinementRegionStats = prevStats->copy();
        }
    } else {
        if (refinementRegionStats->getContextSize() == size) {
            refinementRegionStats->reset();
        } else {
            delete refinementRegionStats;
            refinementRegionStats = new JArithmeticDecoderStats(1 << size);
        }
    }
}