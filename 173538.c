    void PngImage::printStructure(std::ostream& out, PrintStructureOption option, int depth)
    {
        if (io_->open() != 0) {
            throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        }
        if (!isPngType(*io_, true)) {
            throw Error(kerNotAnImage, "PNG");
        }

        char chType[5];
        chType[0] = 0;
        chType[4] = 0;

        if (option == kpsBasic || option == kpsXMP || option == kpsIccProfile || option == kpsRecursive) {
            const std::string xmpKey = "XML:com.adobe.xmp";
            const std::string exifKey = "Raw profile type exif";
            const std::string app1Key = "Raw profile type APP1";
            const std::string iptcKey = "Raw profile type iptc";
            const std::string iccKey = "icc";
            const std::string softKey = "Software";
            const std::string commKey = "Comment";
            const std::string descKey = "Description";

            bool bPrint = option == kpsBasic || option == kpsRecursive;
            if (bPrint) {
                out << "STRUCTURE OF PNG FILE: " << io_->path() << std::endl;
                out << " address | chunk |  length | data                           | checksum" << std::endl;
            }

            const long imgSize = (long)io_->size();
            DataBuf cheaderBuf(8);

            while (!io_->eof() && ::strcmp(chType, "IEND")) {
                size_t address = io_->tell();

                std::memset(cheaderBuf.pData_, 0x0, cheaderBuf.size_);
                long bufRead = io_->read(cheaderBuf.pData_, cheaderBuf.size_);
                if (io_->error())
                    throw Error(kerFailedToReadImageData);
                if (bufRead != cheaderBuf.size_)
                    throw Error(kerInputDataReadFailed);

                // Decode chunk data length.
                uint32_t dataOffset = Exiv2::getULong(cheaderBuf.pData_, Exiv2::bigEndian);
                for (int i = 4; i < 8; i++) {
                    chType[i - 4] = cheaderBuf.pData_[i];
                }

                // test that we haven't hit EOF, or wanting to read excessive data
                long restore = io_->tell();
                if (restore == -1 || dataOffset > uint32_t(0x7FFFFFFF) ||
                    static_cast<long>(dataOffset) > imgSize - restore) {
                    throw Exiv2::Error(kerFailedToReadImageData);
                }

                DataBuf buff(dataOffset);
                io_->read(buff.pData_, dataOffset);
                io_->seek(restore, BasicIo::beg);

                // format output
                const int iMax = 30;
                const uint32_t blen = dataOffset > iMax ? iMax : dataOffset;
                std::string dataString = "";
                // if blen == 0 => slice construction fails
                if (blen > 0) {
                    std::stringstream ss;
                    ss << Internal::binaryToString(makeSlice(buff, 0, blen));
                    dataString = ss.str();
                }
                while (dataString.size() < iMax)
                    dataString += ' ';
                dataString = dataString.substr(0, iMax);

                if (bPrint) {
                    io_->seek(dataOffset, BasicIo::cur);  // jump to checksum
                    byte checksum[4];
                    io_->read(checksum, 4);
                    io_->seek(restore, BasicIo::beg);  // restore file pointer

                    out << Internal::stringFormat("%8d | %-5s |%8d | ", (uint32_t)address, chType, dataOffset)
                        << dataString
                        << Internal::stringFormat(" | 0x%02x%02x%02x%02x", checksum[0], checksum[1], checksum[2],
                                                  checksum[3])
                        << std::endl;
                }

                // chunk type
                bool tEXt = std::strcmp(chType, "tEXt") == 0;
                bool zTXt = std::strcmp(chType, "zTXt") == 0;
                bool iCCP = std::strcmp(chType, "iCCP") == 0;
                bool iTXt = std::strcmp(chType, "iTXt") == 0;

                // for XMP, ICC etc: read and format data
                bool bXMP = option == kpsXMP && findi(dataString, xmpKey) == 0;
                bool bICC = option == kpsIccProfile && findi(dataString, iccKey) == 0;
                bool bExif =
                    option == kpsRecursive && (findi(dataString, exifKey) == 0 || findi(dataString, app1Key) == 0);
                bool bIptc = option == kpsRecursive && findi(dataString, iptcKey) == 0;
                bool bSoft = option == kpsRecursive && findi(dataString, softKey) == 0;
                bool bComm = option == kpsRecursive && findi(dataString, commKey) == 0;
                bool bDesc = option == kpsRecursive && findi(dataString, descKey) == 0;
                bool bDump = bXMP || bICC || bExif || bIptc || bSoft || bComm || bDesc;

                if (bDump) {
                    DataBuf dataBuf;
                    byte* data = new byte[dataOffset + 1];
                    data[dataOffset] = 0;
                    io_->read(data, dataOffset);
                    io_->seek(restore, BasicIo::beg);
                    uint32_t name_l = (uint32_t)std::strlen((const char*)data) + 1;  // leading string length
                    uint32_t start = name_l;

                    // decode the chunk
                    bool bGood = false;
                    if (tEXt) {
                        bGood = tEXtToDataBuf(data + name_l, dataOffset - name_l, dataBuf);
                    }
                    if (zTXt || iCCP) {
                        bGood = zlibToDataBuf(data + name_l + 1, dataOffset - name_l - 1,
                                              dataBuf);  // +1 = 'compressed' flag
                    }
                    if (iTXt) {
                        bGood = (start + 3) < dataOffset;  // good if not a nul chunk
                    }

                    // format is content dependent
                    if (bGood) {
                        bool bLF = false;
                        if (bXMP) {
                            while (!data[start] && start < dataOffset)
                                start++;          // skip leading nul bytes
                            out << data + start;  // output the xmp
                        }

                        if (bExif || bIptc) {
                            DataBuf parsedBuf = PngChunk::readRawProfile(dataBuf, tEXt);
#if DEBUG
                            std::cerr << Exiv2::Internal::binaryToString(parsedBuf.pData_,
                                                                         parsedBuf.size_ > 50 ? 50 : parsedBuf.size_, 0)
                                      << std::endl;
#endif
                            if (parsedBuf.size_) {
                                if (bExif) {
                                    // create memio object with the data, then print the structure
                                    BasicIo::UniquePtr p =
                                        BasicIo::UniquePtr(new MemIo(parsedBuf.pData_ + 6, parsedBuf.size_ - 6));
                                    printTiffStructure(*p, out, option, depth);
                                }
                                if (bIptc) {
                                    IptcData::printStructure(out, makeSlice(parsedBuf.pData_, 0, parsedBuf.size_),
                                                             depth);
                                }
                            }
                        }

                        if (bSoft && dataBuf.size_ > 0) {
                            DataBuf s(dataBuf.size_ + 1);                     // allocate buffer with an extra byte
                            memcpy(s.pData_, dataBuf.pData_, dataBuf.size_);  // copy in the dataBuf
                            s.pData_[dataBuf.size_] = 0;                      // nul terminate it
                            const char* str = (const char*)s.pData_;          // give it name
                            out << Internal::indent(depth) << (const char*)buff.pData_ << ": " << str;
                            bLF = true;
                        }

                        if (bICC || bComm) {
                            out.write((const char*)dataBuf.pData_, dataBuf.size_);
                            bLF = bComm;
                        }

                        if (bDesc && iTXt) {
                            DataBuf decoded = PngChunk::decodeTXTChunk(buff, PngChunk::iTXt_Chunk);
                            out.write((const char*)decoded.pData_, decoded.size_);
                            bLF = true;
                        }

                        if (bLF)
                            out << std::endl;
                    }
                    delete[] data;
                }
                io_->seek(dataOffset + 4, BasicIo::cur);  // jump past checksum
                if (io_->error())
                    throw Error(kerFailedToReadImageData);
            }
        }
    }