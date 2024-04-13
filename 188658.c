Goffset JBIG2Stream::getPos()
{
    if (pageBitmap == nullptr) {
        return 0;
    }
    return dataPtr - pageBitmap->getDataPtr();
}