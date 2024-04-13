    Image::AutoPtr ImageFactory::create(int type)
    {
        BasicIo::AutoPtr io(new MemIo);
        Image::AutoPtr image = create(type, io);
        if (image.get() == 0) throw Error(13, type);
        return image;
    }