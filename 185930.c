    void PngChunk::decodeIHDRChunk(const DataBuf& data,
                                   int*           outWidth,
                                   int*           outHeight)
    {
        assert(data.size_ >= 8);

        // Extract image width and height from IHDR chunk.

        *outWidth  = getLong((const byte*)data.pData_,     bigEndian);
        *outHeight = getLong((const byte*)data.pData_ + 4, bigEndian);

    } // PngChunk::decodeIHDRChunk