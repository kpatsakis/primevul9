std::unique_ptr<JBIG2Bitmap> JBIG2Stream::readTextRegion(bool huff, bool refine, int w, int h, unsigned int numInstances, unsigned int logStrips, int numSyms, const JBIG2HuffmanTable *symCodeTab, unsigned int symCodeLen, JBIG2Bitmap **syms,
                                                         unsigned int defPixel, unsigned int combOp, unsigned int transposed, unsigned int refCorner, int sOffset, const JBIG2HuffmanTable *huffFSTable, const JBIG2HuffmanTable *huffDSTable,
                                                         const JBIG2HuffmanTable *huffDTTable, const JBIG2HuffmanTable *huffRDWTable, const JBIG2HuffmanTable *huffRDHTable, const JBIG2HuffmanTable *huffRDXTable,
                                                         const JBIG2HuffmanTable *huffRDYTable, const JBIG2HuffmanTable *huffRSizeTable, unsigned int templ, int *atx, int *aty)
{
    JBIG2Bitmap *symbolBitmap;
    unsigned int strips;
    int t = 0, dt = 0, tt, s, ds = 0, sFirst, j = 0;
    int rdw, rdh, rdx, rdy, ri = 0, refDX, refDY, bmSize;
    unsigned int symID, inst, bw, bh;

    strips = 1 << logStrips;

    // allocate the bitmap
    std::unique_ptr<JBIG2Bitmap> bitmap = std::make_unique<JBIG2Bitmap>(0, w, h);
    if (!bitmap->isOk()) {
        return nullptr;
    }
    if (defPixel) {
        bitmap->clearToOne();
    } else {
        bitmap->clearToZero();
    }

    // decode initial T value
    if (huff) {
        huffDecoder->decodeInt(&t, huffDTTable);
    } else {
        arithDecoder->decodeInt(&t, iadtStats);
    }

    if (checkedMultiply(t, -(int)strips, &t)) {
        return {};
    }

    inst = 0;
    sFirst = 0;
    while (inst < numInstances) {

        // decode delta-T
        if (huff) {
            huffDecoder->decodeInt(&dt, huffDTTable);
        } else {
            arithDecoder->decodeInt(&dt, iadtStats);
        }
        t += dt * strips;

        // first S value
        if (huff) {
            huffDecoder->decodeInt(&ds, huffFSTable);
        } else {
            arithDecoder->decodeInt(&ds, iafsStats);
        }
        if (unlikely(checkedAdd(sFirst, ds, &sFirst))) {
            return nullptr;
        }
        s = sFirst;

        // read the instances
        // (this loop test is here to avoid an infinite loop with damaged
        // JBIG2 streams where the normal loop exit doesn't get triggered)
        while (inst < numInstances) {

            // T value
            if (strips == 1) {
                dt = 0;
            } else if (huff) {
                dt = huffDecoder->readBits(logStrips);
            } else {
                arithDecoder->decodeInt(&dt, iaitStats);
            }
            if (unlikely(checkedAdd(t, dt, &tt))) {
                return nullptr;
            }

            // symbol ID
            if (huff) {
                if (symCodeTab) {
                    huffDecoder->decodeInt(&j, symCodeTab);
                    symID = (unsigned int)j;
                } else {
                    symID = huffDecoder->readBits(symCodeLen);
                }
            } else {
                if (iaidStats == nullptr) {
                    return nullptr;
                }
                symID = arithDecoder->decodeIAID(symCodeLen, iaidStats);
            }

            if (symID >= (unsigned int)numSyms) {
                error(errSyntaxError, curStr->getPos(), "Invalid symbol number in JBIG2 text region");
                if (unlikely(numInstances - inst > 0x800)) {
                    // don't loop too often with damaged JBIg2 streams
                    return nullptr;
                }
            } else {

                // get the symbol bitmap
                symbolBitmap = nullptr;
                if (refine) {
                    if (huff) {
                        ri = (int)huffDecoder->readBit();
                    } else {
                        arithDecoder->decodeInt(&ri, iariStats);
                    }
                } else {
                    ri = 0;
                }
                if (ri) {
                    bool decodeSuccess;
                    if (huff) {
                        decodeSuccess = huffDecoder->decodeInt(&rdw, huffRDWTable);
                        decodeSuccess = decodeSuccess && huffDecoder->decodeInt(&rdh, huffRDHTable);
                        decodeSuccess = decodeSuccess && huffDecoder->decodeInt(&rdx, huffRDXTable);
                        decodeSuccess = decodeSuccess && huffDecoder->decodeInt(&rdy, huffRDYTable);
                        decodeSuccess = decodeSuccess && huffDecoder->decodeInt(&bmSize, huffRSizeTable);
                        huffDecoder->reset();
                        arithDecoder->start();
                    } else {
                        decodeSuccess = arithDecoder->decodeInt(&rdw, iardwStats);
                        decodeSuccess = decodeSuccess && arithDecoder->decodeInt(&rdh, iardhStats);
                        decodeSuccess = decodeSuccess && arithDecoder->decodeInt(&rdx, iardxStats);
                        decodeSuccess = decodeSuccess && arithDecoder->decodeInt(&rdy, iardyStats);
                    }

                    if (decodeSuccess && syms[symID]) {
                        refDX = ((rdw >= 0) ? rdw : rdw - 1) / 2 + rdx;
                        if (checkedAdd(((rdh >= 0) ? rdh : rdh - 1) / 2, rdy, &refDY)) {
                            return nullptr;
                        }

                        symbolBitmap = readGenericRefinementRegion(rdw + syms[symID]->getWidth(), rdh + syms[symID]->getHeight(), templ, false, syms[symID], refDX, refDY, atx, aty).release();
                    }
                    //~ do we need to use the bmSize value here (in Huffman mode)?
                } else {
                    symbolBitmap = syms[symID];
                }

                if (symbolBitmap) {
                    // combine the symbol bitmap into the region bitmap
                    //~ something is wrong here - refCorner shouldn't degenerate into
                    //~   two cases
                    bw = symbolBitmap->getWidth() - 1;
                    if (unlikely(symbolBitmap->getHeight() == 0)) {
                        error(errSyntaxError, curStr->getPos(), "Invalid symbol bitmap height");
                        if (ri) {
                            delete symbolBitmap;
                        }
                        return nullptr;
                    }
                    bh = symbolBitmap->getHeight() - 1;
                    if (transposed) {
                        if (unlikely(s > 2 * bitmap->getHeight())) {
                            error(errSyntaxError, curStr->getPos(), "Invalid JBIG2 combine");
                            if (ri) {
                                delete symbolBitmap;
                            }
                            return nullptr;
                        }
                        switch (refCorner) {
                        case 0: // bottom left
                            bitmap->combine(symbolBitmap, tt, s, combOp);
                            break;
                        case 1: // top left
                            bitmap->combine(symbolBitmap, tt, s, combOp);
                            break;
                        case 2: // bottom right
                            bitmap->combine(symbolBitmap, tt - bw, s, combOp);
                            break;
                        case 3: // top right
                            bitmap->combine(symbolBitmap, tt - bw, s, combOp);
                            break;
                        }
                        s += bh;
                    } else {
                        switch (refCorner) {
                        case 0: // bottom left
                            if (unlikely(tt - (int)bh > 2 * bitmap->getHeight())) {
                                error(errSyntaxError, curStr->getPos(), "Invalid JBIG2 combine");
                                if (ri) {
                                    delete symbolBitmap;
                                }
                                return nullptr;
                            }
                            bitmap->combine(symbolBitmap, s, tt - bh, combOp);
                            break;
                        case 1: // top left
                            if (unlikely(tt > 2 * bitmap->getHeight())) {
                                error(errSyntaxError, curStr->getPos(), "Invalid JBIG2 combine");
                                if (ri) {
                                    delete symbolBitmap;
                                }
                                return nullptr;
                            }
                            bitmap->combine(symbolBitmap, s, tt, combOp);
                            break;
                        case 2: // bottom right
                            if (unlikely(tt - (int)bh > 2 * bitmap->getHeight())) {
                                error(errSyntaxError, curStr->getPos(), "Invalid JBIG2 combine");
                                if (ri) {
                                    delete symbolBitmap;
                                }
                                return nullptr;
                            }
                            bitmap->combine(symbolBitmap, s, tt - bh, combOp);
                            break;
                        case 3: // top right
                            if (unlikely(tt > 2 * bitmap->getHeight())) {
                                error(errSyntaxError, curStr->getPos(), "Invalid JBIG2 combine");
                                if (ri) {
                                    delete symbolBitmap;
                                }
                                return nullptr;
                            }
                            bitmap->combine(symbolBitmap, s, tt, combOp);
                            break;
                        }
                        s += bw;
                    }
                    if (ri) {
                        delete symbolBitmap;
                    }
                } else {
                    // NULL symbolBitmap only happens on error
                    return nullptr;
                }
            }

            // next instance
            ++inst;

            // next S value
            if (huff) {
                if (!huffDecoder->decodeInt(&ds, huffDSTable)) {
                    break;
                }
            } else {
                if (!arithDecoder->decodeInt(&ds, iadsStats)) {
                    break;
                }
            }
            if (checkedAdd(s, sOffset + ds, &s)) {
                return nullptr;
            }
        }
    }

    return bitmap;
}