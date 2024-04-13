    void TiffImage::setComment(const std::string& /*comment*/)
    {
        // not supported
        throw(Error(32, "Image comment", "TIFF"));
    }