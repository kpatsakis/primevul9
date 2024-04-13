    void PngImage::doWriteMetadata(BasicIo& outIo)
    {
        if (!io_->isopen()) throw Error(20);
        if (!outIo.isopen()) throw Error(21);

#ifdef DEBUG
        std::cout << "Exiv2::PngImage::doWriteMetadata: Writing PNG file " << io_->path() << "\n";
        std::cout << "Exiv2::PngImage::doWriteMetadata: tmp file created " << outIo.path() << "\n";
#endif

        // Ensure that this is the correct image type
        if (!isPngType(*io_, true))
        {
            if (io_->error() || io_->eof()) throw Error(20);
            throw Error(22);
        }

        // Write PNG Signature.
        if (outIo.write(pngSignature, 8) != 8) throw Error(21);

        DataBuf cheaderBuf(8);       // Chunk header : 4 bytes (data size) + 4 bytes (chunk type).

        while(!io_->eof())
        {
            // Read chunk header.

            std::memset(cheaderBuf.pData_, 0x00, cheaderBuf.size_);
            long bufRead = io_->read(cheaderBuf.pData_, cheaderBuf.size_);
            if (io_->error()) throw Error(14);
            if (bufRead != cheaderBuf.size_) throw Error(20);

            // Decode chunk data length.

            uint32_t dataOffset = getULong(cheaderBuf.pData_, bigEndian);
            if (dataOffset > 0x7FFFFFFF) throw Exiv2::Error(14);

            // Read whole chunk : Chunk header + Chunk data (not fixed size - can be null) + CRC (4 bytes).

            DataBuf chunkBuf(8 + dataOffset + 4);                     // Chunk header (8 bytes) + Chunk data + CRC (4 bytes).
            memcpy(chunkBuf.pData_, cheaderBuf.pData_, 8);            // Copy header.
            bufRead = io_->read(chunkBuf.pData_ + 8, dataOffset + 4); // Extract chunk data + CRC
            if (io_->error()) throw Error(14);
            if (bufRead != (long)(dataOffset + 4)) throw Error(20);

            if (!memcmp(cheaderBuf.pData_ + 4, "IEND", 4))
            {
                // Last chunk found: we write it and done.
#ifdef DEBUG
                std::cout << "Exiv2::PngImage::doWriteMetadata: Write IEND chunk (length: " << dataOffset << ")\n";
#endif
                if (outIo.write(chunkBuf.pData_, chunkBuf.size_) != chunkBuf.size_) throw Error(21);
                return;
            }
            else if (!memcmp(cheaderBuf.pData_ + 4, "IHDR", 4))
            {
#ifdef DEBUG
                std::cout << "Exiv2::PngImage::doWriteMetadata: Write IHDR chunk (length: " << dataOffset << ")\n";
#endif
                if (outIo.write(chunkBuf.pData_, chunkBuf.size_) != chunkBuf.size_) throw Error(21);

                // Write all updated metadata here, just after IHDR.
                if (!comment_.empty())
                {
                    // Update Comment data to a new PNG chunk
                    std::string chunk = PngChunk::makeMetadataChunk(comment_, mdComment);
                    if (outIo.write((const byte*)chunk.data(), static_cast<long>(chunk.size())) != (long)chunk.size())
                    {
                        throw Error(21);
                    }
                }

                if (exifData_.count() > 0)
                {
                    // Update Exif data to a new PNG chunk
                    Blob blob;
                    ExifParser::encode(blob, littleEndian, exifData_);
                    if (blob.size() > 0)
                    {
                        static const char exifHeader[] = { 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };
                        std::string rawExif =   std::string(exifHeader, 6)
                                              + std::string((const char*)&blob[0], blob.size());
                        std::string chunk = PngChunk::makeMetadataChunk(rawExif, mdExif);
                        if (outIo.write((const byte*)chunk.data(), static_cast<long>(chunk.size())) != (long)chunk.size())
                        {
                            throw Error(21);
                        }
                    }
                }

                if (iptcData_.count() > 0)
                {
                    // Update IPTC data to a new PNG chunk
                    DataBuf newPsData = Photoshop::setIptcIrb(0, 0, iptcData_);
                    if (newPsData.size_ > 0)
                    {
                        std::string rawIptc((const char*)newPsData.pData_, newPsData.size_);
                        std::string chunk = PngChunk::makeMetadataChunk(rawIptc, mdIptc);
                        if (outIo.write((const byte*)chunk.data(), static_cast<long>(chunk.size())) != (long)chunk.size())
                        {
                            throw Error(21);
                        }
                    }
                }

                if (writeXmpFromPacket() == false) {
                    if (XmpParser::encode(xmpPacket_, xmpData_) > 1) {
#ifndef SUPPRESS_WARNINGS
                        EXV_ERROR << "Failed to encode XMP metadata.\n";
#endif
                    }
                }
                if (xmpPacket_.size() > 0) {
                    // Update XMP data to a new PNG chunk
                    std::string chunk = PngChunk::makeMetadataChunk(xmpPacket_, mdXmp);
                    if (outIo.write((const byte*)chunk.data(), static_cast<long>(chunk.size())) != (long)chunk.size()) {
                        throw Error(21);
                    }
                }
            }
            else if (!memcmp(cheaderBuf.pData_ + 4, "tEXt", 4) ||
                     !memcmp(cheaderBuf.pData_ + 4, "zTXt", 4) ||
                     !memcmp(cheaderBuf.pData_ + 4, "iTXt", 4))
            {
                DataBuf key = PngChunk::keyTXTChunk(chunkBuf, true);
                if (memcmp("Raw profile type exif", key.pData_, 21) == 0 ||
                    memcmp("Raw profile type APP1", key.pData_, 21) == 0 ||
                    memcmp("Raw profile type iptc", key.pData_, 21) == 0 ||
                    memcmp("Raw profile type xmp",  key.pData_, 20) == 0 ||
                    memcmp("XML:com.adobe.xmp",     key.pData_, 17) == 0 ||
                    memcmp("Description",           key.pData_, 11) == 0)
                {
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::doWriteMetadata: strip " << cheaderBuf.pData_ + 4
                              << " chunk (key: " << key.pData_ << ")\n";
#endif
                }
                else
                {
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::doWriteMetadata: write " << cheaderBuf.pData_ + 4
                              << " chunk (length: " << dataOffset << ")\n";
#endif
                    if (outIo.write(chunkBuf.pData_, chunkBuf.size_) != chunkBuf.size_) throw Error(21);
                }
            }
            else
            {
                // Write all others chunk as well.
#ifdef DEBUG
                std::cout << "Exiv2::PngImage::doWriteMetadata: write " << cheaderBuf.pData_ + 4
                          << " chunk (length: " << dataOffset << ")\n";
#endif
                if (outIo.write(chunkBuf.pData_, chunkBuf.size_) != chunkBuf.size_) throw Error(21);

            }
        }

    } // PngImage::doWriteMetadata