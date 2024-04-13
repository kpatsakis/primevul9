    DataBuf PngChunk::keyTXTChunk(const DataBuf& data, bool stripHeader)
    {
        // From a tEXt, zTXt, or iTXt chunk,
        // we get the key, it's a null terminated string at the chunk start
        const int offset = stripHeader ? 8 : 0;
        if (data.size_ <= offset) throw Error(kerFailedToReadImageData);
        const byte *key = data.pData_ + offset;

        // Find null string at end of key.
        int keysize=0;
        while (key[keysize] != 0)
        {
            keysize++;
            // look if keysize is valid.
            if (keysize+offset >= data.size_)
                throw Error(kerFailedToReadImageData);
        }

        return DataBuf(key, keysize);

    } // PngChunk::keyTXTChunk