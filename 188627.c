void JBIG2Stream::readSegments()
{
    unsigned int segNum, segFlags, segType, page, segLength;
    unsigned int refFlags, nRefSegs;
    unsigned int *refSegs;
    int c1, c2, c3;

    bool done = false;
    while (!done && readULong(&segNum)) {

        // segment header flags
        if (!readUByte(&segFlags)) {
            goto eofError1;
        }
        segType = segFlags & 0x3f;

        // referred-to segment count and retention flags
        if (!readUByte(&refFlags)) {
            goto eofError1;
        }
        nRefSegs = refFlags >> 5;
        if (nRefSegs == 7) {
            if ((c1 = curStr->getChar()) == EOF || (c2 = curStr->getChar()) == EOF || (c3 = curStr->getChar()) == EOF) {
                goto eofError1;
            }
            refFlags = (refFlags << 24) | (c1 << 16) | (c2 << 8) | c3;
            nRefSegs = refFlags & 0x1fffffff;
            const unsigned int nCharsToRead = (nRefSegs + 9) >> 3;
            for (unsigned int i = 0; i < nCharsToRead; ++i) {
                if ((c1 = curStr->getChar()) == EOF) {
                    goto eofError1;
                }
            }
        }

        // referred-to segment numbers
        refSegs = (unsigned int *)gmallocn_checkoverflow(nRefSegs, sizeof(unsigned int));
        if (nRefSegs > 0 && !refSegs) {
            return;
        }
        if (segNum <= 256) {
            for (unsigned int i = 0; i < nRefSegs; ++i) {
                if (!readUByte(&refSegs[i])) {
                    goto eofError2;
                }
            }
        } else if (segNum <= 65536) {
            for (unsigned int i = 0; i < nRefSegs; ++i) {
                if (!readUWord(&refSegs[i])) {
                    goto eofError2;
                }
            }
        } else {
            for (unsigned int i = 0; i < nRefSegs; ++i) {
                if (!readULong(&refSegs[i])) {
                    goto eofError2;
                }
            }
        }

        // segment page association
        if (segFlags & 0x40) {
            if (!readULong(&page)) {
                goto eofError2;
            }
        } else {
            if (!readUByte(&page)) {
                goto eofError2;
            }
        }

        // segment data length
        if (!readULong(&segLength)) {
            goto eofError2;
        }

        // check for missing page information segment
        if (!pageBitmap && ((segType >= 4 && segType <= 7) || (segType >= 20 && segType <= 43))) {
            error(errSyntaxError, curStr->getPos(), "First JBIG2 segment associated with a page must be a page information segment");
            goto syntaxError;
        }

        // read the segment data
        arithDecoder->resetByteCounter();
        huffDecoder->resetByteCounter();
        mmrDecoder->resetByteCounter();
        byteCounter = 0;
        switch (segType) {
        case 0:
            if (!readSymbolDictSeg(segNum, segLength, refSegs, nRefSegs)) {
                error(errSyntaxError, curStr->getPos(), "readSymbolDictSeg reports syntax error!");
                goto syntaxError;
            }
            break;
        case 4:
            readTextRegionSeg(segNum, false, false, segLength, refSegs, nRefSegs);
            break;
        case 6:
            readTextRegionSeg(segNum, true, false, segLength, refSegs, nRefSegs);
            break;
        case 7:
            readTextRegionSeg(segNum, true, true, segLength, refSegs, nRefSegs);
            break;
        case 16:
            readPatternDictSeg(segNum, segLength);
            break;
        case 20:
            readHalftoneRegionSeg(segNum, false, false, segLength, refSegs, nRefSegs);
            break;
        case 22:
            readHalftoneRegionSeg(segNum, true, false, segLength, refSegs, nRefSegs);
            break;
        case 23:
            readHalftoneRegionSeg(segNum, true, true, segLength, refSegs, nRefSegs);
            break;
        case 36:
            readGenericRegionSeg(segNum, false, false, segLength);
            break;
        case 38:
            readGenericRegionSeg(segNum, true, false, segLength);
            break;
        case 39:
            readGenericRegionSeg(segNum, true, true, segLength);
            break;
        case 40:
            readGenericRefinementRegionSeg(segNum, false, false, segLength, refSegs, nRefSegs);
            break;
        case 42:
            readGenericRefinementRegionSeg(segNum, true, false, segLength, refSegs, nRefSegs);
            break;
        case 43:
            readGenericRefinementRegionSeg(segNum, true, true, segLength, refSegs, nRefSegs);
            break;
        case 48:
            readPageInfoSeg(segLength);
            break;
        case 50:
            readEndOfStripeSeg(segLength);
            break;
        case 51:
            // end of file segment
            done = true;
            break;
        case 52:
            readProfilesSeg(segLength);
            break;
        case 53:
            readCodeTableSeg(segNum, segLength);
            break;
        case 62:
            readExtensionSeg(segLength);
            break;
        default:
            error(errSyntaxError, curStr->getPos(), "Unknown segment type in JBIG2 stream");
            for (unsigned int i = 0; i < segLength; ++i) {
                if ((c1 = curStr->getChar()) == EOF) {
                    goto eofError2;
                }
            }
            break;
        }

        // Make sure the segment handler read all of the bytes in the
        // segment data, unless this segment is marked as having an
        // unknown length (section 7.2.7 of the JBIG2 Final Committee Draft)

        if (!(segType == 38 && segLength == 0xffffffff)) {

            byteCounter += arithDecoder->getByteCounter();
            byteCounter += huffDecoder->getByteCounter();
            byteCounter += mmrDecoder->getByteCounter();

            if (segLength > byteCounter) {
                const unsigned int segExtraBytes = segLength - byteCounter;

                // If we didn't read all of the bytes in the segment data,
                // indicate an error, and throw away the rest of the data.

                // v.3.1.01.13 of the LuraTech PDF Compressor Server will
                // sometimes generate an extraneous NULL byte at the end of
                // arithmetic-coded symbol dictionary segments when numNewSyms
                // == 0.  Segments like this often occur for blank pages.

                error(errSyntaxError, curStr->getPos(), "{0:ud} extraneous byte{1:s} after segment", segExtraBytes, (segExtraBytes > 1) ? "s" : "");
                byteCounter += curStr->discardChars(segExtraBytes);
            } else if (segLength < byteCounter) {

                // If we read more bytes than we should have, according to the
                // segment length field, note an error.

                error(errSyntaxError, curStr->getPos(), "Previous segment handler read too many bytes");
                goto syntaxError;
            }
        }

        gfree(refSegs);
    }

    return;

syntaxError:
    gfree(refSegs);
    return;

eofError2:
    gfree(refSegs);
eofError1:
    error(errSyntaxError, curStr->getPos(), "Unexpected EOF in JBIG2 stream");
}