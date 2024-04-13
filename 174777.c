    std::string binaryToString(DataBuf& buf, size_t size, size_t start /*=0*/)
    {
        if ( size > (size_t) buf.size_ ) size = (size_t) buf.size_;
        return binaryToString(buf.pData_,size,start);
    }