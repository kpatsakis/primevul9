    void Image::setIccProfile(Exiv2::DataBuf& iccProfile,bool bTestValid)
    {
        if ( bTestValid ) {
            if ( iccProfile.pData_ && ( iccProfile.size_ < (long) sizeof(long)) ) throw Error(53);
            long size = iccProfile.pData_ ? getULong(iccProfile.pData_, bigEndian): -1;
            if ( size!= iccProfile.size_ ) throw Error(53);
        }
        iccProfile_ = iccProfile;
    }