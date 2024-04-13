    Image::AutoPtr ImageFactory::create(int type,
                                        const std::wstring& wpath)
    {
        std::auto_ptr<FileIo> fileIo(new FileIo(wpath));
        // Create or overwrite the file, then close it
        if (fileIo->open("w+b") != 0) {
            throw WError(10, wpath, "w+b", strError().c_str());
        }
        fileIo->close();
        BasicIo::AutoPtr io(fileIo);
        Image::AutoPtr image = create(type, io);
        if (image.get() == 0) throw Error(13, type);
        return image;
    }