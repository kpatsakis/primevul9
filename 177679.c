    void PngImage::printStructure(std::ostream& out, PrintStructureOption option, int depth)
    {
        if (io_->open() != 0) {
            throw Error(9, io_->path(), strError());
        }
        IoCloser closer(*io_);
        // Ensure that this is the correct image type
        if (!isPngType(*io_, true)) {
            if (io_->error() || io_->eof()) throw Error(14);
            throw Error(3, "PNG");
        }

        char    chType[5];
        chType[0]=0;
        chType[4]=0;

        if ( option == kpsBasic || option == kpsXMP || option == kpsIccProfile || option == kpsRecursive ) {

            const std::string xmpKey  = "XML:com.adobe.xmp";
            const std::string exifKey = "Raw profile type exif";
            const std::string iptcKey = "Raw profile type iptc";
            const std::string iccKey  = "icc";
            const std::string softKey = "Software";

            if ( option == kpsBasic || option == kpsRecursive ) {
                out << "STRUCTURE OF PNG FILE: " << io_->path() << std::endl;
                out << " address | index | chunk_type |  length | data" << std::endl;
            }

            long       index   = 0;
            const long imgSize = io_->size();
            DataBuf    cheaderBuf(8);

            while( !io_->eof() && ::strcmp(chType,"IEND") ) {
                size_t address = io_->tell();

                std::memset(cheaderBuf.pData_, 0x0, cheaderBuf.size_);
                long bufRead = io_->read(cheaderBuf.pData_, cheaderBuf.size_);
                if (io_->error()) throw Error(14);
                if (bufRead != cheaderBuf.size_) throw Error(20);

                // Decode chunk data length.
                uint32_t dataOffset = Exiv2::getULong(cheaderBuf.pData_, Exiv2::bigEndian);
                for (int i = 4; i < 8; i++) {
                    chType[i-4]=cheaderBuf.pData_[i];
                }

                // test that we haven't hit EOF, or wanting to read excessive data
                long restore = io_->tell();
                if(  restore == -1
                ||  dataOffset > uint32_t(0x7FFFFFFF)
                ||  static_cast<long>(dataOffset) > imgSize - restore
                ){
                    throw Exiv2::Error(14);
                }

                // format output
                uint32_t    blen = dataOffset > 32 ? 32 : dataOffset ;
                std::string dataString ;
                DataBuf buff(blen);
                io_->read(buff.pData_,blen);
                io_->seek(restore, BasicIo::beg);
                dataString  = Internal::binaryToString(buff, blen);

                if ( option == kpsBasic || option == kpsRecursive )
                    out << Internal::stringFormat("%8d | %5d | %10s |%8d | "
                              ,(uint32_t)address, index++,chType,dataOffset)
                                    << dataString << std::endl;


                // chunk type
                bool tEXt  = std::strcmp(chType,"tEXt")== 0;
                bool zTXt  = std::strcmp(chType,"zTXt")== 0;
                bool iCCP  = std::strcmp(chType,"iCCP")== 0;
                bool iTXt  = std::strcmp(chType,"iTXt")== 0;

                // for XMP, ICC etc: read and format data
                bool bXMP  = option == kpsXMP        && findi(dataString,xmpKey)==0;
                bool bICC  = option == kpsIccProfile && findi(dataString,iccKey)==0;
                bool bExif = option == kpsRecursive  && findi(dataString,exifKey)==0;
                bool bIptc = option == kpsRecursive  && findi(dataString,iptcKey)==0;
                bool bSoft = option == kpsRecursive  && findi(dataString,softKey)==0;
                bool bDump = bXMP || bICC || bExif || bIptc || bSoft ;

                if( bDump ) {
                    DataBuf   dataBuf;
                    byte*     data     = new byte[dataOffset];
                    io_->read(data,dataOffset);
                    io_->seek(restore, BasicIo::beg);
                    uint32_t    name_l = (uint32_t) std::strlen((const char*)data)+1; // leading string length
                    uint32_t     start = name_l;

                    // decode the chunk
                    bool bGood = false;
                    if ( tEXt ) {
                        bGood = tEXtToDataBuf(data+name_l,dataOffset-name_l,dataBuf);
                    }
                    if ( zTXt || iCCP ) {
                        name_l++ ; // +1 = 'compressed' flag
                        bGood = zlibToDataBuf(data+name_l,dataOffset-name_l,dataBuf);
                    }
                    if ( iTXt ) {
                        while ( data[start] == 0 && start < dataOffset ) start++; // crawl over the '\0' bytes between XML:....\0\0<xml stuff
                        data[dataOffset]=0;                 // ensure the XML is nul terminated
                        bGood = (start+3) < dataOffset ;    // good if not a nul chunk
                    }

                    // format is content dependent
                    if ( bGood ) {
                        if ( bXMP ) {
                            out <<  data+start;             // output the xml
                        }
                        if ( bExif ) {
                            const char* bytes = (const char*) dataBuf.pData_;
                            uint32_t    l     = (uint32_t) std::strlen(bytes)+2;
                            // create a copy on write memio object with the data, then print the structure
                            BasicIo::AutoPtr p = BasicIo::AutoPtr(new MemIo(dataBuf.pData_+l,dataBuf.size_-l));
                            TiffImage::printTiffStructure(*p,out,option,depth);
                        }

                        if ( bSoft ) {
                            const char* bytes = (const char*) dataBuf.pData_;
                            uint32_t    l     = (uint32_t) std::strlen(bytes)+2;
                            // create a copy on write memio object with the data, then print the structure
                            BasicIo::AutoPtr p = BasicIo::AutoPtr(new MemIo(dataBuf.pData_+l,dataBuf.size_-l));
                            out << indent(depth) << (const char*) buff.pData_ << ": " << (const char*) dataBuf.pData_ << std::endl;
                        }

                        if ( bICC ) {
                            out.write((const char*) dataBuf.pData_,dataBuf.size_);
                        }

                        if ( bIptc ) {
                            IptcData::printStructure(out,dataBuf.pData_,dataBuf.size_,depth);
                        }
                    }
                    delete [] data;
                }
                io_->seek(dataOffset + 4 , BasicIo::cur);
                if (io_->error()) throw Error(14);
            }
        }
    }