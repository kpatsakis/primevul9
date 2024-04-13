    static bool zlibToDataBuf(const byte* bytes,long length, DataBuf& result)
    {
        uLongf uncompressedLen = length * 2; // just a starting point
        int    zlibResult;

        do {
            result.alloc(uncompressedLen);
            zlibResult = uncompress((Bytef*)result.pData_,&uncompressedLen,bytes,length);
            if (zlibResult == Z_BUF_ERROR) {
                // the uncompressedArray needs to be larger
                result.release();

                // never bigger than 64k
                if  (uncompressedLen > 64*1024) zlibResult = Z_DATA_ERROR;
                uncompressedLen *= 2;
            }
        } while (zlibResult == Z_BUF_ERROR);

        return zlibResult == Z_OK ;
    }