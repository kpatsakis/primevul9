    JpegBase::JpegBase(int type, BasicIo::AutoPtr io, bool create,
                       const byte initData[], long dataSize)
        : Image(type, mdExif | mdIptc | mdXmp | mdComment, io)
    {
        if (create) {
            initImage(initData, dataSize);
        }
    }