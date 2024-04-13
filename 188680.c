bool JBIG2HuffmanDecoder::buildTable(JBIG2HuffmanTable *table, unsigned int len)
{
    unsigned int i, j, k, prefix;
    JBIG2HuffmanTable tab;

    // stable selection sort:
    // - entries with prefixLen > 0, in ascending prefixLen order
    // - entry with prefixLen = 0, rangeLen = EOT
    // - all other entries with prefixLen = 0
    // (on entry, table[len] has prefixLen = 0, rangeLen = EOT)
    for (i = 0; i < len; ++i) {
        for (j = i; j < len && table[j].prefixLen == 0; ++j) {
            ;
        }
        if (j == len) {
            break;
        }
        for (k = j + 1; k < len; ++k) {
            if (table[k].prefixLen > 0 && table[k].prefixLen < table[j].prefixLen) {
                j = k;
            }
        }
        if (j != i) {
            tab = table[j];
            for (k = j; k > i; --k) {
                table[k] = table[k - 1];
            }
            table[i] = tab;
        }
    }
    table[i] = table[len];

    // assign prefixes
    if (table[0].rangeLen != jbig2HuffmanEOT) {
        i = 0;
        prefix = 0;
        table[i++].prefix = prefix++;
        for (; table[i].rangeLen != jbig2HuffmanEOT; ++i) {
            if (table[i].prefixLen - table[i - 1].prefixLen > 32) {
                error(errSyntaxError, -1, "Failed to build table for JBIG2 stream");
                return false;
            } else {
                prefix <<= table[i].prefixLen - table[i - 1].prefixLen;
            }
            table[i].prefix = prefix++;
        }
    }

    return true;
}