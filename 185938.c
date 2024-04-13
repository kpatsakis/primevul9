    void PngChunk::zlibUncompress(const byte*  compressedText,
                                  unsigned int compressedTextSize,
                                  DataBuf&     arr)
    {
        uLongf uncompressedLen = compressedTextSize * 2; // just a starting point
        int zlibResult;
        int dos = 0;

        do {
            arr.alloc(uncompressedLen);
            zlibResult = uncompress((Bytef*)arr.pData_,
                                    &uncompressedLen,
                                    compressedText,
                                    compressedTextSize);
            if (zlibResult == Z_OK) {
                assert((uLongf)arr.size_ >= uncompressedLen);
                arr.size_ = uncompressedLen;
            }
            else if (zlibResult == Z_BUF_ERROR) {
                // the uncompressedArray needs to be larger
                uncompressedLen *= 2;
                // DoS protection. can't be bigger than 64k
                if (uncompressedLen > 131072) {
                    if (++dos > 1) break;
                    uncompressedLen = 131072;
                }
            }
            else {
                // something bad happened
                throw Error(kerFailedToReadImageData);
            }
        }
        while (zlibResult == Z_BUF_ERROR);

        if (zlibResult != Z_OK) {
            throw Error(kerFailedToReadImageData);
        }
    } // PngChunk::zlibUncompress