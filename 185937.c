    std::string PngChunk::zlibCompress(const std::string& text)
    {
        uLongf compressedLen = static_cast<uLongf>(text.size() * 2); // just a starting point
        int zlibResult;

        DataBuf arr;
        do {
            arr.alloc(compressedLen);
            zlibResult = compress2((Bytef*)arr.pData_, &compressedLen,
                                   (const Bytef*)text.data(), static_cast<uLong>(text.size()),
                                   Z_BEST_COMPRESSION);

            switch (zlibResult) {
            case Z_OK:
                assert((uLongf)arr.size_ >= compressedLen);
                arr.size_ = compressedLen;
                break;
            case Z_BUF_ERROR:
                // The compressed array needs to be larger
#ifdef DEBUG
                std::cout << "Exiv2::PngChunk::parsePngChunk: doubling size for compression.\n";
#endif
                compressedLen *= 2;
                // DoS protection. Cap max compressed size
                if ( compressedLen > 131072 ) throw Error(kerFailedToReadImageData);
                break;
            default:
                // Something bad happened
                throw Error(kerFailedToReadImageData);
            }
        } while (zlibResult == Z_BUF_ERROR);

        return std::string((const char*)arr.pData_, arr.size_);

    } // PngChunk::zlibCompress