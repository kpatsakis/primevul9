    Image::AutoPtr newBigTiffInstance(BasicIo::AutoPtr io, bool)
    {
        return Image::AutoPtr(new BigTiffImage(io));
    }