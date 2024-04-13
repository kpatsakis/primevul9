    Image::AutoPtr ImageFactory::create(int type,
                                        const std::string& path)
    {
        std::auto_ptr<FileIo> fileIo(new FileIo(path));
        // Create or overwrite the file, then close it
        if (fileIo->open("w+b") != 0) {
            throw Error(10, path, "w+b", strError());
        }
        fileIo->close();
        BasicIo::AutoPtr io(fileIo);
        Image::AutoPtr image = create(type, io);
        if (image.get() == 0) throw Error(13, type);
        return image;
    }