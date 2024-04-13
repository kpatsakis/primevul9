    Image::AutoPtr newJpegInstance(BasicIo::AutoPtr io, bool create)
    {
        Image::AutoPtr image(new JpegImage(io, create));
        if (!image->good()) {
            image.reset();
        }
        return image;
    }