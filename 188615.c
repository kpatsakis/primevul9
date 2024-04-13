void JBIG2Stream::readPatternDictSeg(unsigned int segNum, unsigned int length)
{
    std::unique_ptr<JBIG2PatternDict> patternDict;
    std::unique_ptr<JBIG2Bitmap> bitmap;
    unsigned int flags, patternW, patternH, grayMax, templ, mmr;
    int atx[4], aty[4];
    unsigned int i, x;

    // halftone dictionary flags, pattern width and height, max gray value
    if (!readUByte(&flags) || !readUByte(&patternW) || !readUByte(&patternH) || !readULong(&grayMax)) {
        goto eofError;
    }
    templ = (flags >> 1) & 3;
    mmr = flags & 1;

    // set up the arithmetic decoder
    if (!mmr) {
        resetGenericStats(templ, nullptr);
        arithDecoder->start();
    }

    // read the bitmap
    atx[0] = -(int)patternW;
    aty[0] = 0;
    atx[1] = -3;
    aty[1] = -1;
    atx[2] = 2;
    aty[2] = -2;
    atx[3] = -2;
    aty[3] = -2;
    bitmap = readGenericBitmap(mmr, (grayMax + 1) * patternW, patternH, templ, false, false, nullptr, atx, aty, length - 7);

    if (!bitmap) {
        return;
    }

    // create the pattern dict object
    patternDict = std::make_unique<JBIG2PatternDict>(segNum, grayMax + 1);

    // split up the bitmap
    x = 0;
    for (i = 0; i <= grayMax && i < patternDict->getSize(); ++i) {
        patternDict->setBitmap(i, bitmap->getSlice(x, 0, patternW, patternH));
        x += patternW;
    }

    // store the new pattern dict
    segments.push_back(std::move(patternDict));

    return;

eofError:
    error(errSyntaxError, curStr->getPos(), "Unexpected EOF in JBIG2 stream");
}