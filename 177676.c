    void PngImage::readMetadata()
    {
#ifdef DEBUG
        std::cerr << "Exiv2::PngImage::readMetadata: Reading PNG file " << io_->path() << "\n";
#endif
        if (io_->open() != 0)
        {
            throw Error(9, io_->path(), strError());
        }
        IoCloser closer(*io_);
        // Ensure that this is the correct image type
        if (!isPngType(*io_, true))
        {
            if (io_->error() || io_->eof()) throw Error(14);
            throw Error(3, "PNG");
        }
        clearMetadata();

        const long imgSize = io_->size();
        DataBuf cheaderBuf(8);       // Chunk header size : 4 bytes (data size) + 4 bytes (chunk type).

        while(!io_->eof())
        {
            // Read chunk header.

#ifdef DEBUG
            std::cout << "Exiv2::PngImage::readMetadata: Position: " << io_->tell() << "\n";
#endif
            std::memset(cheaderBuf.pData_, 0x0, cheaderBuf.size_);
            long bufRead = io_->read(cheaderBuf.pData_, cheaderBuf.size_);
            if (io_->error()) throw Error(14);
            if (bufRead != cheaderBuf.size_) throw Error(20);

            // Decode chunk data length.
            uint32_t dataOffset = Exiv2::getULong(cheaderBuf.pData_, Exiv2::bigEndian);
            long pos = io_->tell();
            if (   pos == -1
                || dataOffset > uint32_t(0x7FFFFFFF)
                || static_cast<long>(dataOffset) > imgSize - pos) throw Exiv2::Error(14);

            // Perform a chunk triage for item that we need.

            if (!memcmp(cheaderBuf.pData_ + 4, "IEND", 4) ||
                !memcmp(cheaderBuf.pData_ + 4, "IHDR", 4) ||
                !memcmp(cheaderBuf.pData_ + 4, "tEXt", 4) ||
                !memcmp(cheaderBuf.pData_ + 4, "zTXt", 4) ||
                !memcmp(cheaderBuf.pData_ + 4, "iTXt", 4))
            {
                // Extract chunk data.

                DataBuf cdataBuf(dataOffset);
                bufRead = io_->read(cdataBuf.pData_, dataOffset);
                if (io_->error()) throw Error(14);
                if (bufRead != (long)dataOffset) throw Error(20);

                if (!memcmp(cheaderBuf.pData_ + 4, "IEND", 4))
                {
                    // Last chunk found: we stop parsing.
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::readMetadata: Found IEND chunk (length: " << dataOffset << ")\n";
#endif
                    return;
                }
                else if (!memcmp(cheaderBuf.pData_ + 4, "IHDR", 4))
                {
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::readMetadata: Found IHDR chunk (length: " << dataOffset << ")\n";
#endif
                    PngChunk::decodeIHDRChunk(cdataBuf, &pixelWidth_, &pixelHeight_);
                }
                else if (!memcmp(cheaderBuf.pData_ + 4, "tEXt", 4))
                {
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::readMetadata: Found tEXt chunk (length: " << dataOffset << ")\n";
#endif
                    PngChunk::decodeTXTChunk(this, cdataBuf, PngChunk::tEXt_Chunk);
                }
                else if (!memcmp(cheaderBuf.pData_ + 4, "zTXt", 4))
                {
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::readMetadata: Found zTXt chunk (length: " << dataOffset << ")\n";
#endif
                    PngChunk::decodeTXTChunk(this, cdataBuf, PngChunk::zTXt_Chunk);
                }
                else if (!memcmp(cheaderBuf.pData_ + 4, "iTXt", 4))
                {
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::readMetadata: Found iTXt chunk (length: " << dataOffset << ")\n";
#endif
                    PngChunk::decodeTXTChunk(this, cdataBuf, PngChunk::iTXt_Chunk);
                }

                // Set dataOffset to null like chunk data have been extracted previously.
                dataOffset = 0;
            }

            // Move to the next chunk: chunk data size + 4 CRC bytes.
#ifdef DEBUG
            std::cout << "Exiv2::PngImage::readMetadata: Seek to offset: " << dataOffset + 4 << "\n";
#endif
            io_->seek(dataOffset + 4 , BasicIo::cur);
            if (io_->error() || io_->eof()) throw Error(14);
        }

    } // PngImage::readMetadata