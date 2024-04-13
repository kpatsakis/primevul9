    std::string PngChunk::makeAsciiTxtChunk(const std::string& keyword,
                                            const std::string& text,
                                            bool               compress)
    {
        // Chunk structure: length (4 bytes) + chunk type + chunk data + CRC (4 bytes)
        // Length is the size of the chunk data
        // CRC is calculated on chunk type + chunk data

        // Compressed text chunk using zlib.
        // Chunk data format : keyword + 0x00 + compression method (0x00) + compressed text

        // Not Compressed text chunk.
        // Chunk data format : keyword + 0x00 + text

        // Build chunk data, determine chunk type
        std::string chunkData = keyword + '\0';
        std::string chunkType;
        if (compress) {
            chunkData += '\0' + zlibCompress(text);
            chunkType = "zTXt";
        }
        else {
            chunkData += text;
            chunkType = "tEXt";
        }
        // Determine length of the chunk data
        byte length[4];
        ul2Data(length, static_cast<uint32_t>(chunkData.size()), bigEndian);
        // Calculate CRC on chunk type and chunk data
        std::string crcData = chunkType + chunkData;
        uLong tmp = crc32(0L, Z_NULL, 0);
        tmp       = crc32(tmp, (const Bytef*)crcData.data(), static_cast<uInt>(crcData.size()));
        byte crc[4];
        ul2Data(crc, tmp, bigEndian);
        // Assemble the chunk
        return std::string((const char*)length, 4) + chunkType + chunkData + std::string((const char*)crc, 4);

    } // PngChunk::makeAsciiTxtChunk