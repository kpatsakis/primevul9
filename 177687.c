    Image::AutoPtr newExvInstance(BasicIo::AutoPtr io, bool create)
    {
        Image::AutoPtr image;
        image = Image::AutoPtr(new ExvImage(io, create));
        if (!image->good()) image.reset();
        return image;
    }