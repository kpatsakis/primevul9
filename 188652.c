void JBIG2Stream::readHalftoneRegionSeg(unsigned int segNum, bool imm, bool lossless, unsigned int length, unsigned int *refSegs, unsigned int nRefSegs)
{
    std::unique_ptr<JBIG2Bitmap> bitmap;
    JBIG2Segment *seg;
    JBIG2PatternDict *patternDict;
    std::unique_ptr<JBIG2Bitmap> skipBitmap;
    unsigned int *grayImg;
    JBIG2Bitmap *patternBitmap;
    unsigned int w, h, x, y, segInfoFlags, extCombOp;
    unsigned int flags, mmr, templ, enableSkip, combOp;
    unsigned int gridW, gridH, stepX, stepY, patW, patH;
    int atx[4], aty[4];
    int gridX, gridY, xx, yy, bit, j;
    unsigned int bpp, m, n, i;

    // region segment info field
    if (!readULong(&w) || !readULong(&h) || !readULong(&x) || !readULong(&y) || !readUByte(&segInfoFlags)) {
        goto eofError;
    }
    extCombOp = segInfoFlags & 7;

    // rest of the halftone region header
    if (!readUByte(&flags)) {
        goto eofError;
    }
    mmr = flags & 1;
    templ = (flags >> 1) & 3;
    enableSkip = (flags >> 3) & 1;
    combOp = (flags >> 4) & 7;
    if (!readULong(&gridW) || !readULong(&gridH) || !readLong(&gridX) || !readLong(&gridY) || !readUWord(&stepX) || !readUWord(&stepY)) {
        goto eofError;
    }
    if (w == 0 || h == 0 || w >= INT_MAX / h) {
        error(errSyntaxError, curStr->getPos(), "Bad bitmap size in JBIG2 halftone segment");
        return;
    }
    if (gridH == 0 || gridW >= INT_MAX / gridH) {
        error(errSyntaxError, curStr->getPos(), "Bad grid size in JBIG2 halftone segment");
        return;
    }

    // get pattern dictionary
    if (nRefSegs != 1) {
        error(errSyntaxError, curStr->getPos(), "Bad symbol dictionary reference in JBIG2 halftone segment");
        return;
    }
    seg = findSegment(refSegs[0]);
    if (seg == nullptr || seg->getType() != jbig2SegPatternDict) {
        error(errSyntaxError, curStr->getPos(), "Bad symbol dictionary reference in JBIG2 halftone segment");
        return;
    }

    patternDict = (JBIG2PatternDict *)seg;
    i = patternDict->getSize();
    if (i <= 1) {
        bpp = 0;
    } else {
        --i;
        bpp = 0;
        // i = floor((size-1) / 2^bpp)
        while (i > 0) {
            ++bpp;
            i >>= 1;
        }
    }
    patternBitmap = patternDict->getBitmap(0);
    if (unlikely(patternBitmap == nullptr)) {
        error(errSyntaxError, curStr->getPos(), "Bad pattern bitmap");
        return;
    }
    patW = patternBitmap->getWidth();
    patH = patternBitmap->getHeight();

    // set up the arithmetic decoder
    if (!mmr) {
        resetGenericStats(templ, nullptr);
        arithDecoder->start();
    }

    // allocate the bitmap
    bitmap = std::make_unique<JBIG2Bitmap>(segNum, w, h);
    if (flags & 0x80) { // HDEFPIXEL
        bitmap->clearToOne();
    } else {
        bitmap->clearToZero();
    }

    // compute the skip bitmap
    if (enableSkip) {
        skipBitmap = std::make_unique<JBIG2Bitmap>(0, gridW, gridH);
        skipBitmap->clearToZero();
        for (m = 0; m < gridH; ++m) {
            for (n = 0; n < gridW; ++n) {
                xx = gridX + m * stepY + n * stepX;
                yy = gridY + m * stepX - n * stepY;
                if (((xx + (int)patW) >> 8) <= 0 || (xx >> 8) >= (int)w || ((yy + (int)patH) >> 8) <= 0 || (yy >> 8) >= (int)h) {
                    skipBitmap->setPixel(n, m);
                }
            }
        }
    }

    // read the gray-scale image
    grayImg = (unsigned int *)gmallocn_checkoverflow(gridW * gridH, sizeof(unsigned int));
    if (!grayImg) {
        return;
    }
    memset(grayImg, 0, gridW * gridH * sizeof(unsigned int));
    atx[0] = templ <= 1 ? 3 : 2;
    aty[0] = -1;
    atx[1] = -3;
    aty[1] = -1;
    atx[2] = 2;
    aty[2] = -2;
    atx[3] = -2;
    aty[3] = -2;
    for (j = bpp - 1; j >= 0; --j) {
        std::unique_ptr<JBIG2Bitmap> grayBitmap = readGenericBitmap(mmr, gridW, gridH, templ, false, enableSkip, skipBitmap.get(), atx, aty, -1);
        i = 0;
        for (m = 0; m < gridH; ++m) {
            for (n = 0; n < gridW; ++n) {
                bit = grayBitmap->getPixel(n, m) ^ (grayImg[i] & 1);
                grayImg[i] = (grayImg[i] << 1) | bit;
                ++i;
            }
        }
    }

    // decode the image
    i = 0;
    for (m = 0; m < gridH; ++m) {
        xx = gridX + m * stepY;
        yy = gridY + m * stepX;
        for (n = 0; n < gridW; ++n) {
            if (!(enableSkip && skipBitmap->getPixel(n, m))) {
                patternBitmap = patternDict->getBitmap(grayImg[i]);
                if (unlikely(patternBitmap == nullptr)) {
                    gfree(grayImg);
                    error(errSyntaxError, curStr->getPos(), "Bad pattern bitmap");
                    return;
                }
                bitmap->combine(patternBitmap, xx >> 8, yy >> 8, combOp);
            }
            xx += stepX;
            yy -= stepY;
            ++i;
        }
    }

    gfree(grayImg);

    // combine the region bitmap into the page bitmap
    if (imm) {
        if (pageH == 0xffffffff && y + h > curPageH) {
            pageBitmap->expand(y + h, pageDefPixel);
        }
        pageBitmap->combine(bitmap.get(), x, y, extCombOp);

        // store the region bitmap
    } else {
        segments.push_back(std::move(bitmap));
    }

    return;

eofError:
    error(errSyntaxError, curStr->getPos(), "Unexpected EOF in JBIG2 stream");
}