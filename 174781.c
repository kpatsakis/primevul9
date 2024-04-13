    Image::AutoPtr ImageFactory::open(BasicIo::AutoPtr io)
    {
        if (io->open() != 0) {
            throw Error(9, io->path(), strError());
        }
        for (unsigned int i = 0; registry[i].imageType_ != ImageType::none; ++i) {
            if (registry[i].isThisType_(*io, false)) {
                return registry[i].newInstance_(io, false);
            }
        }
        return Image::AutoPtr();
    } // ImageFactory::open