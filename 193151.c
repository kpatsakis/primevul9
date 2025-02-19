testHuf (const std::string&)
{
    try
    {
	cout << "Testing Huffman encoder" << endl;

	IMATH_NAMESPACE::Rand48 rand48 (0);

        //
        // FastHufDecoder is used for more than 128 bits, so first test with fewer than 128 bits,
        // then test FastHufDecoder
        //
        for (int pass = 0 ; pass < 2 ; ++pass)
        {

            int N = pass==0 ? 12 : 1000000;
            Array <unsigned short> raw (N);

            fill1 (raw, N, 1, rand48);	  // test various symbol distributions
            compressUncompress (raw, N);
            compressUncompressSubset (raw, N);
            fill1 (raw, N, 10, rand48);
            compressUncompress (raw, N);
            compressUncompressSubset (raw, N);
            fill1 (raw, N, 100, rand48);
            compressUncompress (raw, N);
            compressUncompressSubset (raw, N);
            fill1 (raw, N, 1000, rand48);
            compressUncompress (raw, N);
            compressUncompressSubset (raw, N);

            fill2 (raw, N, 1, rand48);
            compressUncompress (raw, N);
            compressUncompressSubset (raw, N);
            fill2 (raw, N, 10, rand48);
            compressUncompress (raw, N);
            compressUncompressSubset (raw, N);
            fill2 (raw, N, 100, rand48);
            compressUncompress (raw, N);
            compressUncompressSubset (raw, N);
            fill2 (raw, N, 1000, rand48);
            compressUncompress (raw, N);
            compressUncompressSubset (raw, N);

            fill3 (raw, N, 0);
            compressUncompress (raw, N);
            compressUncompressSubset (raw, N);
            fill3 (raw, N, 1);
            compressUncompress (raw, N);
            compressUncompressSubset (raw, N);
            fill3 (raw, N, USHRT_MAX - 1);
            compressUncompress (raw, N);
            compressUncompressSubset (raw, N);
            fill3 (raw, N, USHRT_MAX);
            compressUncompress (raw, N);
            compressUncompressSubset (raw, N);

            if (pass==1)
            {
                fill4 (raw, USHRT_MAX + 1);
                compressVerify(raw, USHRT_MAX + 1, HUF_COMPRESS_DEK_HASH_FOR_FILL4_USHRT_MAX_PLUS_ONE);

                compressUncompress (raw, USHRT_MAX + 1);
                compressUncompressSubset (raw, USHRT_MAX + 1);
                fill4 (raw, N);
                compressVerify(raw, N, HUF_COMPRESS_DEK_HASH_FOR_FILL4_N);
            }
            compressUncompress (raw, N);
            compressUncompressSubset (raw, N);

            fill4 (raw, 0);
            compressUncompress (raw, 0);	// test small input data sets
            fill4 (raw, 1);
            compressUncompress (raw, 1);
            fill4 (raw, 2);
            compressUncompress (raw, 2);
            fill4 (raw, 3);
            compressUncompress (raw, 3);

            fill5 (raw, N);			// test run-length coding of code table
            if (pass==1)
            {
                compressVerify(raw, N, HUF_COMPRESS_DEK_HASH_FOR_FILL5_N);
            }
            compressUncompress (raw, N);
            compressUncompressSubset (raw, N);

        }

	cout << "ok\n" << endl;
    }
    catch (const std::exception &e)
    {
	cerr << "ERROR -- caught exception: " << e.what() << endl;
	assert (false);
    }
}