bool JBIG2Stream::readSymbolDictSeg(unsigned int segNum, unsigned int length, unsigned int *refSegs, unsigned int nRefSegs)
{
    std::unique_ptr<JBIG2SymbolDict> symbolDict;
    const JBIG2HuffmanTable *huffDHTable, *huffDWTable;
    const JBIG2HuffmanTable *huffBMSizeTable, *huffAggInstTable;
    JBIG2Segment *seg;
    std::vector<JBIG2Segment *> codeTables;
    JBIG2SymbolDict *inputSymbolDict;
    unsigned int flags, sdTemplate, sdrTemplate, huff, refAgg;
    unsigned int huffDH, huffDW, huffBMSize, huffAggInst;
    unsigned int contextUsed, contextRetained;
    int sdATX[4], sdATY[4], sdrATX[2], sdrATY[2];
    unsigned int numExSyms, numNewSyms, numInputSyms, symCodeLen;
    JBIG2Bitmap **bitmaps;
    JBIG2Bitmap *collBitmap, *refBitmap;
    unsigned int *symWidths;
    unsigned int symHeight, symWidth, totalWidth, x, symID;
    int dh = 0, dw, refAggNum, refDX = 0, refDY = 0, bmSize;
    bool ex;
    int run, cnt, c;
    unsigned int i, j, k;
    unsigned char *p;

    symWidths = nullptr;

    // symbol dictionary flags
    if (!readUWord(&flags)) {
        goto eofError;
    }
    sdTemplate = (flags >> 10) & 3;
    sdrTemplate = (flags >> 12) & 1;
    huff = flags & 1;
    refAgg = (flags >> 1) & 1;
    huffDH = (flags >> 2) & 3;
    huffDW = (flags >> 4) & 3;
    huffBMSize = (flags >> 6) & 1;
    huffAggInst = (flags >> 7) & 1;
    contextUsed = (flags >> 8) & 1;
    contextRetained = (flags >> 9) & 1;

    // symbol dictionary AT flags
    if (!huff) {
        if (sdTemplate == 0) {
            if (!readByte(&sdATX[0]) || !readByte(&sdATY[0]) || !readByte(&sdATX[1]) || !readByte(&sdATY[1]) || !readByte(&sdATX[2]) || !readByte(&sdATY[2]) || !readByte(&sdATX[3]) || !readByte(&sdATY[3])) {
                goto eofError;
            }
        } else {
            if (!readByte(&sdATX[0]) || !readByte(&sdATY[0])) {
                goto eofError;
            }
        }
    }

    // symbol dictionary refinement AT flags
    if (refAgg && !sdrTemplate) {
        if (!readByte(&sdrATX[0]) || !readByte(&sdrATY[0]) || !readByte(&sdrATX[1]) || !readByte(&sdrATY[1])) {
            goto eofError;
        }
    }

    // SDNUMEXSYMS and SDNUMNEWSYMS
    if (!readULong(&numExSyms) || !readULong(&numNewSyms)) {
        goto eofError;
    }

    // get referenced segments: input symbol dictionaries and code tables
    numInputSyms = 0;
    for (i = 0; i < nRefSegs; ++i) {
        // This is need by bug 12014, returning false makes it not crash
        // but we end up with a empty page while acroread is able to render
        // part of it
        if ((seg = findSegment(refSegs[i]))) {
            if (seg->getType() == jbig2SegSymbolDict) {
                j = ((JBIG2SymbolDict *)seg)->getSize();
                if (numInputSyms > UINT_MAX - j) {
                    error(errSyntaxError, curStr->getPos(), "Too many input symbols in JBIG2 symbol dictionary");
                    goto eofError;
                }
                numInputSyms += j;
            } else if (seg->getType() == jbig2SegCodeTable) {
                codeTables.push_back(seg);
            }
        } else {
            return false;
        }
    }
    if (numInputSyms > UINT_MAX - numNewSyms) {
        error(errSyntaxError, curStr->getPos(), "Too many input symbols in JBIG2 symbol dictionary");
        goto eofError;
    }

    // compute symbol code length, per 6.5.8.2.3
    //  symCodeLen = ceil( log2( numInputSyms + numNewSyms ) )
    i = numInputSyms + numNewSyms;
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

    // get the input symbol bitmaps
    bitmaps = (JBIG2Bitmap **)gmallocn_checkoverflow(numInputSyms + numNewSyms, sizeof(JBIG2Bitmap *));
    if (!bitmaps && (numInputSyms + numNewSyms > 0)) {
        error(errSyntaxError, curStr->getPos(), "Too many input symbols in JBIG2 symbol dictionary");
        goto eofError;
    }
    for (i = 0; i < numInputSyms + numNewSyms; ++i) {
        bitmaps[i] = nullptr;
    }
    k = 0;
    inputSymbolDict = nullptr;
    for (i = 0; i < nRefSegs; ++i) {
        seg = findSegment(refSegs[i]);
        if (seg != nullptr && seg->getType() == jbig2SegSymbolDict) {
            inputSymbolDict = (JBIG2SymbolDict *)seg;
            for (j = 0; j < inputSymbolDict->getSize(); ++j) {
                bitmaps[k++] = inputSymbolDict->getBitmap(j);
            }
        }
    }

    // get the Huffman tables
    huffDHTable = huffDWTable = nullptr; // make gcc happy
    huffBMSizeTable = huffAggInstTable = nullptr; // make gcc happy
    i = 0;
    if (huff) {
        if (huffDH == 0) {
            huffDHTable = huffTableD;
        } else if (huffDH == 1) {
            huffDHTable = huffTableE;
        } else {
            if (i >= codeTables.size()) {
                goto codeTableError;
            }
            huffDHTable = ((JBIG2CodeTable *)codeTables[i++])->getHuffTable();
        }
        if (huffDW == 0) {
            huffDWTable = huffTableB;
        } else if (huffDW == 1) {
            huffDWTable = huffTableC;
        } else {
            if (i >= codeTables.size()) {
                goto codeTableError;
            }
            huffDWTable = ((JBIG2CodeTable *)codeTables[i++])->getHuffTable();
        }
        if (huffBMSize == 0) {
            huffBMSizeTable = huffTableA;
        } else {
            if (i >= codeTables.size()) {
                goto codeTableError;
            }
            huffBMSizeTable = ((JBIG2CodeTable *)codeTables[i++])->getHuffTable();
        }
        if (huffAggInst == 0) {
            huffAggInstTable = huffTableA;
        } else {
            if (i >= codeTables.size()) {
                goto codeTableError;
            }
            huffAggInstTable = ((JBIG2CodeTable *)codeTables[i++])->getHuffTable();
        }
    }

    // set up the Huffman decoder
    if (huff) {
        huffDecoder->reset();

        // set up the arithmetic decoder
    } else {
        if (contextUsed && inputSymbolDict) {
            resetGenericStats(sdTemplate, inputSymbolDict->getGenericRegionStats());
        } else {
            resetGenericStats(sdTemplate, nullptr);
        }
        if (!resetIntStats(symCodeLen)) {
            goto syntaxError;
        }
        arithDecoder->start();
    }

    // set up the arithmetic decoder for refinement/aggregation
    if (refAgg) {
        if (contextUsed && inputSymbolDict) {
            resetRefinementStats(sdrTemplate, inputSymbolDict->getRefinementRegionStats());
        } else {
            resetRefinementStats(sdrTemplate, nullptr);
        }
    }

    // allocate symbol widths storage
    if (huff && !refAgg) {
        symWidths = (unsigned int *)gmallocn_checkoverflow(numNewSyms, sizeof(unsigned int));
        if (numNewSyms > 0 && !symWidths) {
            goto syntaxError;
        }
    }

    symHeight = 0;
    i = 0;
    while (i < numNewSyms) {

        // read the height class delta height
        if (huff) {
            huffDecoder->decodeInt(&dh, huffDHTable);
        } else {
            arithDecoder->decodeInt(&dh, iadhStats);
        }
        if (dh < 0 && (unsigned int)-dh >= symHeight) {
            error(errSyntaxError, curStr->getPos(), "Bad delta-height value in JBIG2 symbol dictionary");
            goto syntaxError;
        }
        symHeight += dh;
        if (unlikely(symHeight > 0x40000000)) {
            error(errSyntaxError, curStr->getPos(), "Bad height value in JBIG2 symbol dictionary");
            goto syntaxError;
        }
        symWidth = 0;
        totalWidth = 0;
        j = i;

        // read the symbols in this height class
        while (true) {

            // read the delta width
            if (huff) {
                if (!huffDecoder->decodeInt(&dw, huffDWTable)) {
                    break;
                }
            } else {
                if (!arithDecoder->decodeInt(&dw, iadwStats)) {
                    break;
                }
            }
            if (dw < 0 && (unsigned int)-dw >= symWidth) {
                error(errSyntaxError, curStr->getPos(), "Bad delta-height value in JBIG2 symbol dictionary");
                goto syntaxError;
            }
            symWidth += dw;
            if (i >= numNewSyms) {
                error(errSyntaxError, curStr->getPos(), "Too many symbols in JBIG2 symbol dictionary");
                goto syntaxError;
            }

            // using a collective bitmap, so don't read a bitmap here
            if (huff && !refAgg) {
                symWidths[i] = symWidth;
                totalWidth += symWidth;

                // refinement/aggregate coding
            } else if (refAgg) {
                if (huff) {
                    if (!huffDecoder->decodeInt(&refAggNum, huffAggInstTable)) {
                        break;
                    }
                } else {
                    if (!arithDecoder->decodeInt(&refAggNum, iaaiStats)) {
                        break;
                    }
                }
                //~ This special case was added about a year before the final draft
                //~ of the JBIG2 spec was released.  I have encountered some old
                //~ JBIG2 images that predate it.
                //~ if (0) {
                if (refAggNum == 1) {
                    if (huff) {
                        symID = huffDecoder->readBits(symCodeLen);
                        huffDecoder->decodeInt(&refDX, huffTableO);
                        huffDecoder->decodeInt(&refDY, huffTableO);
                        huffDecoder->decodeInt(&bmSize, huffTableA);
                        huffDecoder->reset();
                        arithDecoder->start();
                    } else {
                        if (iaidStats == nullptr) {
                            goto syntaxError;
                        }
                        symID = arithDecoder->decodeIAID(symCodeLen, iaidStats);
                        arithDecoder->decodeInt(&refDX, iardxStats);
                        arithDecoder->decodeInt(&refDY, iardyStats);
                    }
                    if (symID >= numInputSyms + i) {
                        error(errSyntaxError, curStr->getPos(), "Invalid symbol ID in JBIG2 symbol dictionary");
                        goto syntaxError;
                    }
                    refBitmap = bitmaps[symID];
                    if (unlikely(refBitmap == nullptr)) {
                        error(errSyntaxError, curStr->getPos(), "Invalid ref bitmap for symbol ID {0:ud} in JBIG2 symbol dictionary", symID);
                        goto syntaxError;
                    }
                    bitmaps[numInputSyms + i] = readGenericRefinementRegion(symWidth, symHeight, sdrTemplate, false, refBitmap, refDX, refDY, sdrATX, sdrATY).release();
                    //~ do we need to use the bmSize value here (in Huffman mode)?
                } else {
                    bitmaps[numInputSyms + i] = readTextRegion(huff, true, symWidth, symHeight, refAggNum, 0, numInputSyms + i, nullptr, symCodeLen, bitmaps, 0, 0, 0, 1, 0, huffTableF, huffTableH, huffTableK, huffTableO, huffTableO,
                                                               huffTableO, huffTableO, huffTableA, sdrTemplate, sdrATX, sdrATY)
                                                        .release();
                    if (unlikely(!bitmaps[numInputSyms + i])) {
                        error(errSyntaxError, curStr->getPos(), "NULL bitmap in readTextRegion");
                        goto syntaxError;
                    }
                }

                // non-ref/agg coding
            } else {
                bitmaps[numInputSyms + i] = readGenericBitmap(false, symWidth, symHeight, sdTemplate, false, false, nullptr, sdATX, sdATY, 0).release();
                if (unlikely(!bitmaps[numInputSyms + i])) {
                    error(errSyntaxError, curStr->getPos(), "NULL bitmap in readGenericBitmap");
                    goto syntaxError;
                }
            }

            ++i;
        }

        // read the collective bitmap
        if (huff && !refAgg) {
            huffDecoder->decodeInt(&bmSize, huffBMSizeTable);
            huffDecoder->reset();
            if (bmSize == 0) {
                collBitmap = new JBIG2Bitmap(0, totalWidth, symHeight);
                bmSize = symHeight * ((totalWidth + 7) >> 3);
                p = collBitmap->getDataPtr();
                if (unlikely(p == nullptr)) {
                    delete collBitmap;
                    goto syntaxError;
                }
                for (k = 0; k < (unsigned int)bmSize; ++k) {
                    if ((c = curStr->getChar()) == EOF) {
                        memset(p, 0, bmSize - k);
                        break;
                    }
                    *p++ = (unsigned char)c;
                }
            } else {
                collBitmap = readGenericBitmap(true, totalWidth, symHeight, 0, false, false, nullptr, nullptr, nullptr, bmSize).release();
            }
            if (likely(collBitmap != nullptr)) {
                x = 0;
                for (; j < i; ++j) {
                    bitmaps[numInputSyms + j] = collBitmap->getSlice(x, 0, symWidths[j], symHeight);
                    x += symWidths[j];
                }
                delete collBitmap;
            } else {
                error(errSyntaxError, curStr->getPos(), "collBitmap was null");
                goto syntaxError;
            }
        }
    }

    // create the symbol dict object
    symbolDict = std::make_unique<JBIG2SymbolDict>(segNum, numExSyms);
    if (!symbolDict->isOk()) {
        goto syntaxError;
    }

    // exported symbol list
    i = j = 0;
    ex = false;
    run = 0; // initialize it once in case the first decodeInt fails
             // we do not want to use uninitialized memory
    while (i < numInputSyms + numNewSyms) {
        if (huff) {
            huffDecoder->decodeInt(&run, huffTableA);
        } else {
            arithDecoder->decodeInt(&run, iaexStats);
        }
        if (i + run > numInputSyms + numNewSyms || (ex && j + run > numExSyms)) {
            error(errSyntaxError, curStr->getPos(), "Too many exported symbols in JBIG2 symbol dictionary");
            for (; j < numExSyms; ++j) {
                symbolDict->setBitmap(j, nullptr);
            }
            goto syntaxError;
        }
        if (ex) {
            for (cnt = 0; cnt < run; ++cnt) {
                symbolDict->setBitmap(j++, new JBIG2Bitmap(bitmaps[i++]));
            }
        } else {
            i += run;
        }
        ex = !ex;
    }
    if (j != numExSyms) {
        error(errSyntaxError, curStr->getPos(), "Too few symbols in JBIG2 symbol dictionary");
        for (; j < numExSyms; ++j) {
            symbolDict->setBitmap(j, nullptr);
        }
        goto syntaxError;
    }

    for (i = 0; i < numNewSyms; ++i) {
        delete bitmaps[numInputSyms + i];
    }
    gfree(bitmaps);
    if (symWidths) {
        gfree(symWidths);
    }

    // save the arithmetic decoder stats
    if (!huff && contextRetained) {
        symbolDict->setGenericRegionStats(genericRegionStats->copy());
        if (refAgg) {
            symbolDict->setRefinementRegionStats(refinementRegionStats->copy());
        }
    }

    // store the new symbol dict
    segments.push_back(std::move(symbolDict));

    return true;

codeTableError:
    error(errSyntaxError, curStr->getPos(), "Missing code table in JBIG2 symbol dictionary");

syntaxError:
    for (i = 0; i < numNewSyms; ++i) {
        if (bitmaps[numInputSyms + i]) {
            delete bitmaps[numInputSyms + i];
        }
    }
    gfree(bitmaps);
    if (symWidths) {
        gfree(symWidths);
    }
    return false;

eofError:
    error(errSyntaxError, curStr->getPos(), "Unexpected EOF in JBIG2 stream");
    return false;
}