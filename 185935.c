    std::string PngChunk::makeUtf8TxtChunk(const std::string& keyword,
                                           const std::string& text,
                                           bool               compress)
    {
        // Chunk structure: length (4 bytes) + chunk type + chunk data + CRC (4 bytes)
        // Length is the size of the chunk data
        // CRC is calculated on chunk type + chunk data

        // Chunk data format : keyword + 0x00 + compression flag (0x00: uncompressed - 0x01: compressed)
        //                     + compression method (0x00: zlib format) + language tag (null) + 0x00
        //                     + translated keyword (null) + 0x00 + text (compressed or not)

        // Build chunk data, determine chunk type
        std::string chunkData = keyword;
        if (compress) {
            static const char flags[] = { 0x00, 0x01, 0x00, 0x00, 0x00 };
            chunkData += std::string(flags, 5) + zlibCompress(text);
        }
        else {
            static const char flags[] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
            chunkData += std::string(flags, 5) + text;
        }
        // Determine length of the chunk data
        byte length[4];
        ul2Data(length, static_cast<uint32_t>(chunkData.size()), bigEndian);
        // Calculate CRC on chunk type and chunk data
        std::string chunkType = "iTXt";
        std::string crcData = chunkType + chunkData;
        uLong tmp = crc32(0L, Z_NULL, 0);
        tmp       = crc32(tmp, (const Bytef*)crcData.data(), static_cast<uInt>(crcData.size()));
        byte crc[4];
        ul2Data(crc, tmp, bigEndian);
        // Assemble the chunk
        return std::string((const char*)length, 4) + chunkType + chunkData + std::string((const char*)crc, 4);

    } // PngChunk::makeUtf8TxtChunk