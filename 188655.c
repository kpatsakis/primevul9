JBIG2Segment *JBIG2Stream::findSegment(unsigned int segNum)
{
    for (std::unique_ptr<JBIG2Segment> &seg : globalSegments) {
        if (seg->getSegNum() == segNum) {
            return seg.get();
        }
    }
    for (std::unique_ptr<JBIG2Segment> &seg : segments) {
        if (seg->getSegNum() == segNum) {
            return seg.get();
        }
    }
    return nullptr;
}