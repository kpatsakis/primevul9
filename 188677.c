bool JBIG2Stream::resetIntStats(int symCodeLen)
{
    iadhStats->reset();
    iadwStats->reset();
    iaexStats->reset();
    iaaiStats->reset();
    iadtStats->reset();
    iaitStats->reset();
    iafsStats->reset();
    iadsStats->reset();
    iardxStats->reset();
    iardyStats->reset();
    iardwStats->reset();
    iardhStats->reset();
    iariStats->reset();
    if (symCodeLen + 1 >= 31) {
        return false;
    }
    if (iaidStats != nullptr && iaidStats->getContextSize() == 1 << (symCodeLen + 1)) {
        iaidStats->reset();
    } else {
        delete iaidStats;
        iaidStats = new JArithmeticDecoderStats(1 << (symCodeLen + 1));
        if (!iaidStats->isValid()) {
            delete iaidStats;
            iaidStats = nullptr;
            return false;
        }
    }
    return true;
}