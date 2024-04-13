void JBIG2Stream::readGenericRefinementRegionSeg(unsigned int segNum, bool imm, bool lossless, unsigned int length, unsigned int *refSegs, unsigned int nRefSegs)
{
    std::unique_ptr<JBIG2Bitmap> bitmap;
    JBIG2Bitmap *refBitmap;
    unsigned int w, h, x, y, segInfoFlags, extCombOp;
    unsigned int flags, templ, tpgrOn;
    int atx[2], aty[2];
    JBIG2Segment *seg;

    // region segment info field
    if (!readULong(&w) || !readULong(&h) || !readULong(&x) || !readULong(&y) || !readUByte(&segInfoFlags)) {
        goto eofError;
    }
    extCombOp = segInfoFlags & 7;

    // rest of the generic refinement region segment header
    if (!readUByte(&flags)) {
        goto eofError;
    }
    templ = flags & 1;
    tpgrOn = (flags >> 1) & 1;

    // AT flags
    if (!templ) {
        if (!readByte(&atx[0]) || !readByte(&aty[0]) || !readByte(&atx[1]) || !readByte(&aty[1])) {
            goto eofError;
        }
    }

    // resize the page bitmap if needed
    if (nRefSegs == 0 || imm) {
        if (pageH == 0xffffffff && y + h > curPageH) {
            pageBitmap->expand(y + h, pageDefPixel);
        }
    }

    // get referenced bitmap
    if (nRefSegs > 1) {
        error(errSyntaxError, curStr->getPos(), "Bad reference in JBIG2 generic refinement segment");
        return;
    }
    if (nRefSegs == 1) {
        seg = findSegment(refSegs[0]);
        if (seg == nullptr || seg->getType() != jbig2SegBitmap) {
            error(errSyntaxError, curStr->getPos(), "Bad bitmap reference in JBIG2 generic refinement segment");
            return;
        }
        refBitmap = (JBIG2Bitmap *)seg;
    } else {
        refBitmap = pageBitmap->getSlice(x, y, w, h);
    }

    // set up the arithmetic decoder
    resetRefinementStats(templ, nullptr);
    arithDecoder->start();

    // read
    bitmap = readGenericRefinementRegion(w, h, templ, tpgrOn, refBitmap, 0, 0, atx, aty);

    // combine the region bitmap into the page bitmap
    if (imm && bitmap) {
        pageBitmap->combine(bitmap.get(), x, y, extCombOp);

        // store the region bitmap
    } else {
        if (bitmap) {
            bitmap->setSegNum(segNum);
            segments.push_back(std::move(bitmap));
        } else {
            error(errSyntaxError, curStr->getPos(), "readGenericRefinementRegionSeg with null bitmap");
        }
    }

    // delete the referenced bitmap
    if (nRefSegs == 1) {
        discardSegment(refSegs[0]);
    } else {
        delete refBitmap;
    }

    return;

eofError:
    error(errSyntaxError, curStr->getPos(), "Unexpected EOF in JBIG2 stream");
}