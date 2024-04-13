    void TiffImage::writeMetadata()
    {
#ifdef DEBUG
        std::cerr << "Writing TIFF file " << io_->path() << "\n";
#endif
        ByteOrder bo = byteOrder();
        byte* pData = 0;
        long size = 0;
        IoCloser closer(*io_);
        if (io_->open() == 0) {
            // Ensure that this is the correct image type
            if (isTiffType(*io_, false)) {
                pData = io_->mmap(true);
                size = io_->size();
                TiffHeader tiffHeader;
                if (0 == tiffHeader.read(pData, 8)) {
                    bo = tiffHeader.byteOrder();
                }
            }
        }
        if (bo == invalidByteOrder) {
            bo = littleEndian;
        }
        setByteOrder(bo);
        TiffParser::encode(*io_, pData, size, bo, exifData_, iptcData_, xmpData_); // may throw
    } // TiffImage::writeMetadata