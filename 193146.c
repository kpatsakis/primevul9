compressUncompress (const unsigned short raw[], int n)
{
    Array <char> compressed (3 * n + 4 * 65536);
    Array <unsigned short> uncompressed (n);

    cout << "compressing " << flush;

    int nCompressed = hufCompress (raw, n, compressed);

    cout << "uncompressing " << flush;

    hufUncompress (compressed, nCompressed, uncompressed, n);

    cout << "comparing: " << flush;

    for (int i = 0; i < n; ++i)
	assert (uncompressed[i] == raw[i]);

    cout << sizeof (raw[0]) * n << " bytes, compressed " <<
	    nCompressed  << " bytes" << endl;
}