    DataBuf TiffHeaderBase::write() const
    {
        DataBuf buf(8);
        switch (byteOrder_) {
        case littleEndian:
            buf.pData_[0] = 0x49;
            buf.pData_[1] = 0x49;
            break;
        case bigEndian:
            buf.pData_[0] = 0x4d;
            buf.pData_[1] = 0x4d;
            break;
        case invalidByteOrder:
            assert(false);
            break;
        }
        us2Data(buf.pData_ + 2, tag_, byteOrder_);
        ul2Data(buf.pData_ + 4, 0x00000008, byteOrder_);
        return buf;
    }