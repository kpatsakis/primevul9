    static uint16_t byteSwap2(DataBuf& buf,size_t offset,bool bSwap)
    {
        uint16_t v;
        char*    p = (char*) &v;
        p[0] = buf.pData_[offset];
        p[1] = buf.pData_[offset+1];
        return byteSwap(v,bSwap);
    }