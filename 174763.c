    Image::AutoPtr ImageFactory::open(const byte* data, long size)
    {
        BasicIo::AutoPtr io(new MemIo(data, size));
        Image::AutoPtr image = open(io); // may throw
        if (image.get() == 0) throw Error(12);
        return image;
    }