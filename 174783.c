                BigTiffImage(BasicIo::AutoPtr io):
                    Image(ImageType::bigtiff, mdExif, io),
                    header_(),
                    dataSize_(0),
                    doSwap_(false)
                {
                    header_ = readHeader(Image::io());
                    assert(header_.isValid());

                    doSwap_ =  (isLittleEndianPlatform() && header_.byteOrder() == bigEndian)
                          ||   (isBigEndianPlatform()    && header_.byteOrder() == littleEndian);

                    dataSize_ = header_.format() == Header::StandardTiff? 4 : 8;
                }