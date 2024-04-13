void JBIG2Stream::discardSegment(unsigned int segNum)
{
    for (auto it = globalSegments.begin(); it != globalSegments.end(); ++it) {
        if ((*it)->getSegNum() == segNum) {
            globalSegments.erase(it);
            return;
        }
    }
    for (auto it = segments.begin(); it != segments.end(); ++it) {
        if ((*it)->getSegNum() == segNum) {
            segments.erase(it);
            return;
        }
    }
}