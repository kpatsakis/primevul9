JBIG2Stream::~JBIG2Stream()
{
    close();
    delete arithDecoder;
    delete genericRegionStats;
    delete refinementRegionStats;
    delete iadhStats;
    delete iadwStats;
    delete iaexStats;
    delete iaaiStats;
    delete iadtStats;
    delete iaitStats;
    delete iafsStats;
    delete iadsStats;
    delete iardxStats;
    delete iardyStats;
    delete iardwStats;
    delete iardhStats;
    delete iariStats;
    delete iaidStats;
    delete huffDecoder;
    delete mmrDecoder;
    delete str;
}