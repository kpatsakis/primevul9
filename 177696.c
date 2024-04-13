    ExvImage::ExvImage(BasicIo::AutoPtr io, bool create)
        : JpegBase(ImageType::exv, io, create, blank_, sizeof(blank_))
    {
    }