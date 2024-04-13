inline void JBIG2Stream::mmrAddPixelsNeg(int a1, int blackPixels, int *codingLine, int *a0i, int w)
{
    if (a1 > codingLine[*a0i]) {
        if (a1 > w) {
            error(errSyntaxError, curStr->getPos(), "JBIG2 MMR row is wrong length ({0:d})", a1);
            a1 = w;
        }
        if ((*a0i & 1) ^ blackPixels) {
            ++*a0i;
        }
        codingLine[*a0i] = a1;
    } else if (a1 < codingLine[*a0i]) {
        if (a1 < 0) {
            error(errSyntaxError, curStr->getPos(), "Invalid JBIG2 MMR code");
            a1 = 0;
        }
        while (*a0i > 0 && a1 <= codingLine[*a0i - 1]) {
            --*a0i;
        }
        codingLine[*a0i] = a1;
    }
}