void JBIG2Stream::readTextRegionSeg(unsigned int segNum, bool imm, bool lossless, unsigned int length, unsigned int *refSegs, unsigned int nRefSegs)
{
    std::unique_ptr<JBIG2Bitmap> bitmap;
    JBIG2HuffmanTable runLengthTab[36];
    JBIG2HuffmanTable *symCodeTab = nullptr;
    const JBIG2HuffmanTable *huffFSTable, *huffDSTable, *huffDTTable;
    const JBIG2HuffmanTable *huffRDWTable, *huffRDHTable;
    const JBIG2HuffmanTable *huffRDXTable, *huffRDYTable, *huffRSizeTable;
    JBIG2Segment *seg;
    std::vector<JBIG2Segment *> codeTables;
    JBIG2SymbolDict *symbolDict;
    JBIG2Bitmap **syms;
    unsigned int w, h, x, y, segInfoFlags, extCombOp;
    unsigned int flags, huff, refine, logStrips, refCorner, transposed;
    unsigned int combOp, defPixel, templ;
    int sOffset;
    unsigned int huffFlags, huffFS, huffDS, huffDT;
    unsigned int huffRDW, huffRDH, huffRDX, huffRDY, huffRSize;
    unsigned int numInstances, numSyms, symCodeLen;
    int atx[2], aty[2];
    unsigned int i, k, kk;
    int j = 0;

    // region segment info field
    if (!readULong(&w) || !readULong(&h) || !readULong(&x) || !readULong(&y) || !readUByte(&segInfoFlags)) {
        goto eofError;
    }
    extCombOp = segInfoFlags & 7;

    // rest of the text region header
    if (!readUWord(&flags)) {
        goto eofError;
    }
    huff = flags & 1;
    refine = (flags >> 1) & 1;
    logStrips = (flags >> 2) & 3;
    refCorner = (flags >> 4) & 3;
    transposed = (flags >> 6) & 1;
    combOp = (flags >> 7) & 3;
    defPixel = (flags >> 9) & 1;
    sOffset = (flags >> 10) & 0x1f;
    if (sOffset & 0x10) {
        sOffset |= -1 - 0x0f;
    }
    templ = (flags >> 15) & 1;
    huffFS = huffDS = huffDT = 0; // make gcc happy
    huffRDW = huffRDH = huffRDX = huffRDY = huffRSize = 0; // make gcc happy
    if (huff) {
        if (!readUWord(&huffFlags)) {
            goto eofError;
        }
        huffFS = huffFlags & 3;
        huffDS = (huffFlags >> 2) & 3;
        huffDT = (huffFlags >> 4) & 3;
        huffRDW = (huffFlags >> 6) & 3;
        huffRDH = (huffFlags >> 8) & 3;
        huffRDX = (huffFlags >> 10) & 3;
        huffRDY = (huffFlags >> 12) & 3;
        huffRSize = (huffFlags >> 14) & 1;
    }
    if (refine && templ == 0) {
        if (!readByte(&atx[0]) || !readByte(&aty[0]) || !readByte(&atx[1]) || !readByte(&aty[1])) {
            goto eofError;
        }
    }
    if (!readULong(&numInstances)) {
        goto eofError;
    }

    // get symbol dictionaries and tables
    numSyms = 0;
    for (i = 0; i < nRefSegs; ++i) {
        if ((seg = findSegment(refSegs[i]))) {
            if (seg->getType() == jbig2SegSymbolDict) {
                const unsigned int segSize = ((JBIG2SymbolDict *)seg)->getSize();
                if (unlikely(checkedAdd(numSyms, segSize, &numSyms))) {
                    error(errSyntaxError, getPos(), "Too many symbols in JBIG2 text region");
                    return;
                }
            } else if (seg->getType() == jbig2SegCodeTable) {
                codeTables.push_back(seg);
            }
        } else {
            error(errSyntaxError, curStr->getPos(), "Invalid segment reference in JBIG2 text region");
            return;
        }
    }
    i = numSyms;
    if (i <= 1) {
        symCodeLen = huff ? 1 : 0;
    } else {
        --i;
        symCodeLen = 0;
        // i = floor((numSyms-1) / 2^symCodeLen)
        while (i > 0) {
            ++symCodeLen;
            i >>= 1;
        }
    }

    // get the symbol bitmaps
    syms = (JBIG2Bitmap **)gmallocn_checkoverflow(numSyms, sizeof(JBIG2Bitmap *));
    if (numSyms > 0 && !syms) {
        return;
    }
    kk = 0;
    for (i = 0; i < nRefSegs; ++i) {
        if ((seg = findSegment(refSegs[i]))) {
            if (seg->getType() == jbig2SegSymbolDict) {
                symbolDict = (JBIG2SymbolDict *)seg;
                for (k = 0; k < symbolDict->getSize(); ++k) {
                    syms[kk++] = symbolDict->getBitmap(k);
                }
            }
        }
    }

    // get the Huffman tables
    huffFSTable = huffDSTable = huffDTTable = nullptr; // make gcc happy
    huffRDWTable = huffRDHTable = nullptr; // make gcc happy
    huffRDXTable = huffRDYTable = huffRSizeTable = nullptr; // make gcc happy
    i = 0;
    if (huff) {
        if (huffFS == 0) {
            huffFSTable = huffTableF;
        } else if (huffFS == 1) {
            huffFSTable = huffTableG;
        } else {
            if (i >= codeTables.size()) {
                goto codeTableError;
            }
            huffFSTable = ((JBIG2CodeTable *)codeTables[i++])->getHuffTable();
        }
        if (huffDS == 0) {
            huffDSTable = huffTableH;
        } else if (huffDS == 1) {
            huffDSTable = huffTableI;
        } else if (huffDS == 2) {
            huffDSTable = huffTableJ;
        } else {
            if (i >= codeTables.size()) {
                goto codeTableError;
            }
            huffDSTable = ((JBIG2CodeTable *)codeTables[i++])->getHuffTable();
        }
        if (huffDT == 0) {
            huffDTTable = huffTableK;
        } else if (huffDT == 1) {
            huffDTTable = huffTableL;
        } else if (huffDT == 2) {
            huffDTTable = huffTableM;
        } else {
            if (i >= codeTables.size()) {
                goto codeTableError;
            }
            huffDTTable = ((JBIG2CodeTable *)codeTables[i++])->getHuffTable();
        }
        if (huffRDW == 0) {
            huffRDWTable = huffTableN;
        } else if (huffRDW == 1) {
            huffRDWTable = huffTableO;
        } else {
            if (i >= codeTables.size()) {
                goto codeTableError;
            }
            huffRDWTable = ((JBIG2CodeTable *)codeTables[i++])->getHuffTable();
        }
        if (huffRDH == 0) {
            huffRDHTable = huffTableN;
        } else if (huffRDH == 1) {
            huffRDHTable = huffTableO;
        } else {
            if (i >= codeTables.size()) {
                goto codeTableError;
            }
            huffRDHTable = ((JBIG2CodeTable *)codeTables[i++])->getHuffTable();
        }
        if (huffRDX == 0) {
            huffRDXTable = huffTableN;
        } else if (huffRDX == 1) {
            huffRDXTable = huffTableO;
        } else {
            if (i >= codeTables.size()) {
                goto codeTableError;
            }
            huffRDXTable = ((JBIG2CodeTable *)codeTables[i++])->getHuffTable();
        }
        if (huffRDY == 0) {
            huffRDYTable = huffTableN;
        } else if (huffRDY == 1) {
            huffRDYTable = huffTableO;
        } else {
            if (i >= codeTables.size()) {
                goto codeTableError;
            }
            huffRDYTable = ((JBIG2CodeTable *)codeTables[i++])->getHuffTable();
        }
        if (huffRSize == 0) {
            huffRSizeTable = huffTableA;
        } else {
            if (i >= codeTables.size()) {
                goto codeTableError;
            }
            huffRSizeTable = ((JBIG2CodeTable *)codeTables[i++])->getHuffTable();
        }
    }

    // symbol ID Huffman decoding table
    if (huff) {
        huffDecoder->reset();
        for (i = 0; i < 32; ++i) {
            runLengthTab[i].val = i;
            runLengthTab[i].prefixLen = huffDecoder->readBits(4);
            runLengthTab[i].rangeLen = 0;
        }
        runLengthTab[32].val = 0x103;
        runLengthTab[32].prefixLen = huffDecoder->readBits(4);
        runLengthTab[32].rangeLen = 2;
        runLengthTab[33].val = 0x203;
        runLengthTab[33].prefixLen = huffDecoder->readBits(4);
        runLengthTab[33].rangeLen = 3;
        runLengthTab[34].val = 0x20b;
        runLengthTab[34].prefixLen = huffDecoder->readBits(4);
        runLengthTab[34].rangeLen = 7;
        runLengthTab[35].prefixLen = 0;
        runLengthTab[35].rangeLen = jbig2HuffmanEOT;
        if (!JBIG2HuffmanDecoder::buildTable(runLengthTab, 35)) {
            huff = false;
        }
    }

    if (huff) {
        symCodeTab = (JBIG2HuffmanTable *)gmallocn_checkoverflow(numSyms + 1, sizeof(JBIG2HuffmanTable));
        if (!symCodeTab) {
            gfree(syms);
            return;
        }
        for (i = 0; i < numSyms; ++i) {
            symCodeTab[i].val = i;
            symCodeTab[i].rangeLen = 0;
        }
        i = 0;
        while (i < numSyms) {
            huffDecoder->decodeInt(&j, runLengthTab);
            if (j > 0x200) {
                for (j -= 0x200; j && i < numSyms; --j) {
                    symCodeTab[i++].prefixLen = 0;
                }
            } else if (j > 0x100) {
                if (unlikely(i == 0)) {
                    symCodeTab[i].prefixLen = 0;
                    ++i;
                }
                for (j -= 0x100; j && i < numSyms; --j) {
                    symCodeTab[i].prefixLen = symCodeTab[i - 1].prefixLen;
                    ++i;
                }
            } else {
                symCodeTab[i++].prefixLen = j;
            }
        }
        symCodeTab[numSyms].prefixLen = 0;
        symCodeTab[numSyms].rangeLen = jbig2HuffmanEOT;
        if (!JBIG2HuffmanDecoder::buildTable(symCodeTab, numSyms)) {
            huff = false;
            gfree(symCodeTab);
            symCodeTab = nullptr;
        }
        huffDecoder->reset();

        // set up the arithmetic decoder
    }

    if (!huff) {
        if (!resetIntStats(symCodeLen)) {
            gfree(syms);
            return;
        }
        arithDecoder->start();
    }
    if (refine) {
        resetRefinementStats(templ, nullptr);
    }

    bitmap = readTextRegion(huff, refine, w, h, numInstances, logStrips, numSyms, symCodeTab, symCodeLen, syms, defPixel, combOp, transposed, refCorner, sOffset, huffFSTable, huffDSTable, huffDTTable, huffRDWTable, huffRDHTable,
                            huffRDXTable, huffRDYTable, huffRSizeTable, templ, atx, aty);

    gfree(syms);

    if (bitmap) {
        // combine the region bitmap into the page bitmap
        if (imm) {
            if (pageH == 0xffffffff && y + h > curPageH) {
                pageBitmap->expand(y + h, pageDefPixel);
            }
            if (pageBitmap->isOk()) {
                pageBitmap->combine(bitmap.get(), x, y, extCombOp);
            }

            // store the region bitmap
        } else {
            bitmap->setSegNum(segNum);
            segments.push_back(std::move(bitmap));
        }
    }

    // clean up the Huffman decoder
    if (huff) {
        gfree(symCodeTab);
    }

    return;

codeTableError:
    error(errSyntaxError, curStr->getPos(), "Missing code table in JBIG2 text region");
    gfree(syms);
    return;

eofError:
    error(errSyntaxError, curStr->getPos(), "Unexpected EOF in JBIG2 stream");
    return;
}