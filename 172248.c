    void WebPImage::readMetadata()
    {
        if (io_->open() != 0) throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        IoCloser closer(*io_);
        // Ensure that this is the correct image type
        if (!isWebPType(*io_, true)) {
            if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
            throw Error(kerNotAJpeg);
        }
        clearMetadata();

        byte data[12];
        DataBuf chunkId(5);
        chunkId.pData_[4] = '\0' ;

        readOrThrow(*io_, data, WEBP_TAG_SIZE * 3, Exiv2::kerCorruptedMetadata);

        const uint32_t filesize = Exiv2::getULong(data + WEBP_TAG_SIZE, littleEndian) + 8;
        enforce(filesize <= io_->size(), Exiv2::kerCorruptedMetadata);
        WebPImage::decodeChunks(filesize);

    } // WebPImage::readMetadata