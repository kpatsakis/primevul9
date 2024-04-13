    JpegImage::JpegImage(BasicIo::AutoPtr io, bool create)
        : JpegBase(ImageType::jpeg, io, create, blank_, sizeof(blank_))
    {
    }