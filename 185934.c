    void PngChunk::decodeTXTChunk(Image*         pImage,
                                  const DataBuf& data,
                                  TxtChunkType   type)
    {
        DataBuf key = keyTXTChunk(data);
        DataBuf arr = parseTXTChunk(data, key.size_, type);

#ifdef DEBUG
        std::cout << "Exiv2::PngChunk::decodeTXTChunk: TXT chunk data: "
                  << std::string((const char*)arr.pData_, arr.size_) << std::endl;
#endif
        parseChunkContent(pImage, key.pData_, key.size_, arr);

    } // PngChunk::decodeTXTChunk