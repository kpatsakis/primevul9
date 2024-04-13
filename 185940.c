    DataBuf PngChunk::decodeTXTChunk(const DataBuf& data,
                                     TxtChunkType   type)
    {
        DataBuf key = keyTXTChunk(data);

#ifdef DEBUG
        std::cout << "Exiv2::PngChunk::decodeTXTChunk: TXT chunk key: "
                  << std::string((const char*)key.pData_, key.size_) << std::endl;
#endif
        return parseTXTChunk(data, key.size_, type);

    } // PngChunk::decodeTXTChunk