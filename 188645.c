void JBIG2Stream::readCodeTableSeg(unsigned int segNum, unsigned int length)
{
    JBIG2HuffmanTable *huffTab;
    unsigned int flags, oob, prefixBits, rangeBits;
    int lowVal, highVal, val;
    unsigned int huffTabSize, i;

    if (!readUByte(&flags) || !readLong(&lowVal) || !readLong(&highVal)) {
        goto eofError;
    }
    oob = flags & 1;
    prefixBits = ((flags >> 1) & 7) + 1;
    rangeBits = ((flags >> 4) & 7) + 1;

    huffDecoder->reset();
    huffTabSize = 8;
    huffTab = (JBIG2HuffmanTable *)gmallocn_checkoverflow(huffTabSize, sizeof(JBIG2HuffmanTable));
    if (unlikely(!huffTab)) {
        goto oomError;
    }

    i = 0;
    val = lowVal;
    while (val < highVal) {
        if (i == huffTabSize) {
            huffTabSize *= 2;
            huffTab = (JBIG2HuffmanTable *)greallocn_checkoverflow(huffTab, huffTabSize, sizeof(JBIG2HuffmanTable));
            if (unlikely(!huffTab)) {
                goto oomError;
            }
        }
        huffTab[i].val = val;
        huffTab[i].prefixLen = huffDecoder->readBits(prefixBits);
        huffTab[i].rangeLen = huffDecoder->readBits(rangeBits);
        if (unlikely(checkedAdd(val, 1 << huffTab[i].rangeLen, &val))) {
            free(huffTab);
            return;
        }
        ++i;
    }
    if (i + oob + 3 > huffTabSize) {
        huffTabSize = i + oob + 3;
        huffTab = (JBIG2HuffmanTable *)greallocn_checkoverflow(huffTab, huffTabSize, sizeof(JBIG2HuffmanTable));
        if (unlikely(!huffTab)) {
            goto oomError;
        }
    }
    huffTab[i].val = lowVal - 1;
    huffTab[i].prefixLen = huffDecoder->readBits(prefixBits);
    huffTab[i].rangeLen = jbig2HuffmanLOW;
    ++i;
    huffTab[i].val = highVal;
    huffTab[i].prefixLen = huffDecoder->readBits(prefixBits);
    huffTab[i].rangeLen = 32;
    ++i;
    if (oob) {
        huffTab[i].val = 0;
        huffTab[i].prefixLen = huffDecoder->readBits(prefixBits);
        huffTab[i].rangeLen = jbig2HuffmanOOB;
        ++i;
    }
    huffTab[i].val = 0;
    huffTab[i].prefixLen = 0;
    huffTab[i].rangeLen = jbig2HuffmanEOT;
    if (JBIG2HuffmanDecoder::buildTable(huffTab, i)) {
        // create and store the new table segment
        segments.push_back(std::make_unique<JBIG2CodeTable>(segNum, huffTab));
    } else {
        free(huffTab);
    }

    return;

eofError:
    error(errSyntaxError, curStr->getPos(), "Unexpected EOF in JBIG2 stream");
oomError:
    error(errInternal, curStr->getPos(), "Failed allocation when processing JBIG2 stream");
}