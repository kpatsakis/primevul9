void JBIG2Stream::close()
{
    if (pageBitmap) {
        delete pageBitmap;
        pageBitmap = nullptr;
    }
    segments.resize(0);
    globalSegments.resize(0);
    dataPtr = dataEnd = nullptr;
    FilterStream::close();
}