compressUncompressSubset(const unsigned short raw[], int n)
{
    // Test various subsets of the data set

    Array <char> compressed (3 * n + 4 * 65536);
    Array <unsigned short> uncompressed (n);

    int maxOffset = 16;
    if (n <= maxOffset) {
        maxOffset = n-1;
    }

    for (int offset=1; offset<maxOffset; ++offset) {

        int maxLength = n - offset;
        int minLength = maxLength - 16;
        if (minLength < 1) minLength = 1;

        for (int length=minLength; length<=maxLength; ++length) {
            cout << "compressing " << flush;

            int nCompressed = hufCompress (raw + offset, length, compressed + offset);

            cout << "uncompressing " << flush;

            hufUncompress (compressed + offset, nCompressed, uncompressed + offset, length);

            for (int i = 0; i < length; ++i)
                assert (uncompressed[offset+i] == raw[offset+i]);

            cout << sizeof (raw[0]) * length << " bytes, compressed " << nCompressed  << " bytes ";
            cout << "(+" << offset << ",-" << n-offset-length << ")" << endl;
        }
    }
}