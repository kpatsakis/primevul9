    void TiffImage::printIFDStructure(BasicIo& io, std::ostream& out, Exiv2::PrintStructureOption option,uint32_t start,bool bSwap,char c,int depth)
    {
        depth++;
        bool bFirst  = true  ;

        // buffer
        const size_t dirSize = 32;
        DataBuf  dir(dirSize);
        bool bPrint = option == kpsBasic || option == kpsRecursive;

        do {
            // Read top of directory
            io.seek(start,BasicIo::beg);
            io.read(dir.pData_, 2);
            uint16_t   dirLength = byteSwap2(dir,0,bSwap);

            bool tooBig = dirLength > 500;

            if ( bFirst && bPrint ) {
                out << indent(depth) << Internal::stringFormat("STRUCTURE OF TIFF FILE (%c%c): ",c,c) << io.path() << std::endl;
                if ( tooBig ) out << indent(depth) << "dirLength = " << dirLength << std::endl;
            }
            if  (tooBig) break;

            // Read the dictionary
            for ( int i = 0 ; i < dirLength ; i ++ ) {
                if ( bFirst && bPrint ) {
                    out << indent(depth)
                        << " address |    tag                           |     "
                        << " type |    count |    offset | value\n";
                }
                bFirst = false;

                io.read(dir.pData_, 12);
                uint16_t tag    = byteSwap2(dir,0,bSwap);
                uint16_t type   = byteSwap2(dir,2,bSwap);
                uint32_t count  = byteSwap4(dir,4,bSwap);
                uint32_t offset = byteSwap4(dir,8,bSwap);

                std::string sp  = "" ; // output spacer

                //prepare to print the value
                uint16_t kount  = isPrintXMP(tag,option) ? count // restrict long arrays
                                : isPrintICC(tag,option) ? count //
                                : isStringType(type)     ? (count > 32 ? 32 : count)
                                : count > 5              ? 5
                                : count
                                ;
                uint32_t pad    = isStringType(type) ? 1 : 0;
                uint32_t size   = isStringType(type) ? 1
                                : is2ByteType(type)  ? 2
                                : is4ByteType(type)  ? 4
                                : 1
                                ;

                // if ( offset > io.size() ) offset = 0;  // Denial of service?
                DataBuf  buf(MIN(size*kount + pad,48));  // allocate a buffer
                if ( isStringType(type) || count*size > 4 ) {          // data is in the directory => read into buffer
                    size_t   restore = io.tell();  // save
                    io.seek(offset,BasicIo::beg);  // position
                    io.read(buf.pData_,kount*size);// read
                    io.seek(restore,BasicIo::beg); // restore
                } else {                     // move data from directory to the buffer
                    std::memcpy(buf.pData_,dir.pData_+8,12);
                }

                uint32_t Offset = isLongType(type) ? byteSwap4(buf,0,bSwap) : 0 ;

                if ( bPrint ) {
                    uint32_t address = start + 2 + i*12 ;
                    out << indent(depth)
                            << Internal::stringFormat("%8u | %#06x %-25s |%10s |%9u |%10u | "
                                ,address,tag,tagName(tag,25),typeName(type),count,offset);
                    if ( isShortType(type) ){
                        for ( uint16_t k = 0 ; k < kount ; k++ ) {
                            out << sp << byteSwap2(buf,k*size,bSwap);
                            sp = " ";
                        }
                    } else if ( isLongType(type) ){
                        for ( uint16_t k = 0 ; k < kount ; k++ ) {
                            out << sp << byteSwap4(buf,k*size,bSwap);
                            sp = " ";
                        }
                    } else if ( isRationalType(type) ){
                        for ( uint16_t k = 0 ; k < kount ; k++ ) {
                            uint16_t a = byteSwap2(buf,k*size+0,bSwap);
                            uint16_t b = byteSwap2(buf,k*size+2,bSwap);
                            if ( isLittleEndian() ) {
                                if ( bSwap ) out << sp << b << "/" << a;
                                else         out << sp << a << "/" << b;
                            } else {
                                if ( bSwap ) out << sp << a << "/" << b;
                                else         out << sp << b << "/" << a;
                            }
                            sp = " ";
                        }
                    } else if ( isStringType(type) ) {
                        out << sp << Internal::binaryToString(buf, kount);
                    }

                    sp = kount == count ? "" : " ...";
                    out << sp << std::endl;
                    if ( option == kpsRecursive && tag == 0x8769 /* ExifTag */ ) {
                        size_t restore = io.tell();
                        printIFDStructure(io,out,option,Offset,bSwap,c,depth);
                        io.seek(restore,BasicIo::beg);
                    } else if ( option == kpsRecursive && tag == 0x83bb /* IPTCNAA */ ) {
                        size_t   restore = io.tell();  // save
                        io.seek(offset,BasicIo::beg);  // position
                        byte* bytes=new byte[count] ;  // allocate memory
                        io.read(bytes,count)        ;  // read
                        io.seek(restore,BasicIo::beg); // restore
                        IptcData::printStructure(out,bytes,count,depth);
                        delete[] bytes;                // free
                    }
                }

                if ( isPrintXMP(tag,option) ) {
                    buf.pData_[count]=0;
                    out << (char*) buf.pData_;
                }
                if ( isPrintICC(tag,option) ) {
                    out.write((const char*)buf.pData_,buf.size_);
                }
            }
            io.read(dir.pData_, 4);
            start = tooBig ? 0 : byteSwap4(dir,0,bSwap);
            out.flush();
        } while (start) ;

        if ( bPrint ) {
            out << indent(depth) << "END " << io.path() << std::endl;
        }
        depth--;
    }