    static uint32_t byteSwap4(DataBuf& buf,size_t offset,bool bSwap)
    {
        uint32_t v;
        char*    p = (char*) &v;
        p[0] = buf.pData_[offset];
        p[1] = buf.pData_[offset+1];
        p[2] = buf.pData_[offset+2];
        p[3] = buf.pData_[offset+3];
        return byteSwap(v,bSwap);
    }