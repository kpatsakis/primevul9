static void family_init(QuicFamily *family, int bpc, int limit)
{
    int l, b;

    for (l = 0; l < bpc; l++) { /* fill arrays indexed by code number */
        int altprefixlen, altcodewords;

        altprefixlen = limit - bpc;
        if (altprefixlen > (int)(bppmask[bpc - l])) {
            altprefixlen = bppmask[bpc - l];
        }

        altcodewords = bppmask[bpc] + 1 - (altprefixlen << l);

        family->nGRcodewords[l] = (altprefixlen << l);
        family->notGRsuffixlen[l] = ceil_log_2(altcodewords); /* needed for decoding only */
        family->notGRcwlen[l] = altprefixlen + family->notGRsuffixlen[l];
        family->notGRprefixmask[l] = bppmask[32 - altprefixlen]; /* needed for decoding only */

        for (b = 0; b < 256; b++) {
            unsigned int code, len;
            golomb_coding_slow(family, b, l, &code, &len);
            family->golomb_code[b][l] = code;
            family->golomb_code_len[b][l] = len;
        }
    }

    decorrelate_init(family, bpc);
    correlate_init(family, bpc);
}