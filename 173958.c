static void golomb_coding_slow(const QuicFamily *family, const BYTE n, const unsigned int l,
                               unsigned int * const codeword,
                               unsigned int * const codewordlen)
{
    if (n < family->nGRcodewords[l]) {
        (*codeword) = bitat(l) | (n & bppmask[l]);
        (*codewordlen) = (n >> l) + l + 1;
    } else {
        (*codeword) = n - family->nGRcodewords[l];
        (*codewordlen) = family->notGRcwlen[l];
    }
}