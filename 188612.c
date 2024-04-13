void JBIG2Stream::readPageInfoSeg(unsigned int length)
{
    unsigned int xRes, yRes, flags, striping;

    if (!readULong(&pageW) || !readULong(&pageH) || !readULong(&xRes) || !readULong(&yRes) || !readUByte(&flags) || !readUWord(&striping)) {
        goto eofError;
    }
    pageDefPixel = (flags >> 2) & 1;
    defCombOp = (flags >> 3) & 3;

    // allocate the page bitmap
    if (pageH == 0xffffffff) {
        curPageH = striping & 0x7fff;
    } else {
        curPageH = pageH;
    }
    delete pageBitmap;
    pageBitmap = new JBIG2Bitmap(0, pageW, curPageH);

    if (!pageBitmap->isOk()) {
        delete pageBitmap;
        pageBitmap = nullptr;
        return;
    }

    // default pixel value
    if (pageDefPixel) {
        pageBitmap->clearToOne();
    } else {
        pageBitmap->clearToZero();
    }

    return;

eofError:
    error(errSyntaxError, curStr->getPos(), "Unexpected EOF in JBIG2 stream");
}