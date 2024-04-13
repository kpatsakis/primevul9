    Image::AutoPtr ImageFactory::open(const std::wstring& wpath, bool useCurl)
    {
        Image::AutoPtr image = open(ImageFactory::createIo(wpath, useCurl)); // may throw
        if (image.get() == 0) throw WError(11, wpath);
        return image;
    }