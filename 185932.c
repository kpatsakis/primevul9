    void PngChunk::parseChunkContent(      Image*  pImage,
                                     const byte*   key,
                                           long    keySize,
                                     const DataBuf arr)
    {
        // We look if an ImageMagick EXIF raw profile exist.

        if (   keySize >= 21
            && (   memcmp("Raw profile type exif", key, 21) == 0
                || memcmp("Raw profile type APP1", key, 21) == 0)
            && pImage->exifData().empty())
        {
            DataBuf exifData = readRawProfile(arr,false);
            long length      = exifData.size_;

            if (length > 0)
            {
                // Find the position of Exif header in bytes array.

                const byte exifHeader[] = { 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };
                long pos = -1;

                for (long i=0 ; i < length-(long)sizeof(exifHeader) ; i++)
                {
                    if (memcmp(exifHeader, &exifData.pData_[i], sizeof(exifHeader)) == 0)
                    {
                        pos = i;
                        break;
                    }
                }

                // If found it, store only these data at from this place.

                if (pos !=-1)
                {
#ifdef DEBUG
                    std::cout << "Exiv2::PngChunk::parseChunkContent: Exif header found at position " << pos << "\n";
#endif
                    pos = pos + sizeof(exifHeader);
                    ByteOrder bo = TiffParser::decode(pImage->exifData(),
                                                      pImage->iptcData(),
                                                      pImage->xmpData(),
                                                      exifData.pData_ + pos,
                                                      length - pos);
                    pImage->setByteOrder(bo);
                }
                else
                {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to decode Exif metadata.\n";
#endif
                    pImage->exifData().clear();
                }
            }
        }

        // We look if an ImageMagick IPTC raw profile exist.

        if (   keySize >= 21
            && memcmp("Raw profile type iptc", key, 21) == 0
            && pImage->iptcData().empty()) {
            DataBuf psData = readRawProfile(arr,false);
            if (psData.size_ > 0) {
                Blob iptcBlob;
                const byte *record = 0;
                uint32_t sizeIptc = 0;
                uint32_t sizeHdr = 0;

                const byte* pEnd = psData.pData_ + psData.size_;
                const byte* pCur = psData.pData_;
                while (   pCur < pEnd
                       && 0 == Photoshop::locateIptcIrb(pCur,
                                                        static_cast<long>(pEnd - pCur),
                                                        &record,
                                                        &sizeHdr,
                                                        &sizeIptc)) {
                    if (sizeIptc) {
#ifdef DEBUG
                        std::cerr << "Found IPTC IRB, size = " << sizeIptc << "\n";
#endif
                        append(iptcBlob, record + sizeHdr, sizeIptc);
                    }
                    pCur = record + sizeHdr + sizeIptc;
                    pCur += (sizeIptc & 1);
                }
                if (   iptcBlob.size() > 0
                    && IptcParser::decode(pImage->iptcData(),
                                          &iptcBlob[0],
                                          static_cast<uint32_t>(iptcBlob.size()))) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to decode IPTC metadata.\n";
#endif
                    pImage->clearIptcData();
                }
                // If there is no IRB, try to decode the complete chunk data
                if (   iptcBlob.empty()
                    && IptcParser::decode(pImage->iptcData(),
                                          psData.pData_,
                                          psData.size_)) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to decode IPTC metadata.\n";
#endif
                    pImage->clearIptcData();
                }
            } // if (psData.size_ > 0)
        }

        // We look if an ImageMagick XMP raw profile exist.

        if (   keySize >= 20
            && memcmp("Raw profile type xmp", key, 20) == 0
            && pImage->xmpData().empty())
        {
            DataBuf xmpBuf = readRawProfile(arr,false);
            long length    = xmpBuf.size_;

            if (length > 0)
            {
                std::string& xmpPacket = pImage->xmpPacket();
                xmpPacket.assign(reinterpret_cast<char*>(xmpBuf.pData_), length);
                std::string::size_type idx = xmpPacket.find_first_of('<');
                if (idx != std::string::npos && idx > 0)
                {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Removing " << idx
                                << " characters from the beginning of the XMP packet\n";
#endif
                    xmpPacket = xmpPacket.substr(idx);
                }
                if (XmpParser::decode(pImage->xmpData(), xmpPacket))
                {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to decode XMP metadata.\n";
#endif
                }
            }
        }

        // We look if an Adobe XMP string exist.

        if (   keySize >= 17
            && memcmp("XML:com.adobe.xmp", key, 17) == 0
            && pImage->xmpData().empty())
        {
            if (arr.size_ > 0)
            {
                std::string& xmpPacket = pImage->xmpPacket();
                xmpPacket.assign(reinterpret_cast<char*>(arr.pData_), arr.size_);
                std::string::size_type idx = xmpPacket.find_first_of('<');
                if (idx != std::string::npos && idx > 0)
                {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Removing " << idx << " characters "
                                << "from the beginning of the XMP packet\n";
#endif
                    xmpPacket = xmpPacket.substr(idx);
                }
                if (XmpParser::decode(pImage->xmpData(), xmpPacket))
                {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to decode XMP metadata.\n";
#endif
                }
            }
        }

        // We look if a comments string exist. Note than we use only 'Description' keyword which
        // is dedicaced to store long comments. 'Comment' keyword is ignored.

        if (   keySize >= 11
            && memcmp("Description", key, 11) == 0
            && pImage->comment().empty())
        {
            pImage->setComment(std::string(reinterpret_cast<char*>(arr.pData_), arr.size_));
        }

    } // PngChunk::parseChunkContent