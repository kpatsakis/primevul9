    DataBuf PngChunk::readRawProfile(const DataBuf& text,bool iTXt)
    {
        DataBuf                 info;
        register long           i;
        register unsigned char *dp;
        const char             *sp;
        unsigned int            nibbles;
        long                    length;
        unsigned char           unhex[103]={0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,1, 2,3,4,5,6,7,8,9,0,0,
                                            0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,10,11,12,
                                            13,14,15};
        if (text.size_ == 0) {
            return DataBuf();
        }

        if ( iTXt ) {
            info.alloc(text.size_);
            ::memcpy(info.pData_,text.pData_,text.size_);
            return  info;
        }


        sp = (char*)text.pData_+1;

        // Look for newline

        while (*sp != '\n')
            sp++;

        // Look for length

        while (*sp == '\0' || *sp == ' ' || *sp == '\n')
            sp++;

        length = (long) atol(sp);

        while (*sp != ' ' && *sp != '\n')
            sp++;

        // Allocate space

        if (length == 0)
        {
#ifdef DEBUG
            std::cerr << "Exiv2::PngChunk::readRawProfile: Unable To Copy Raw Profile: invalid profile length\n";
#endif
        }

        info.alloc(length);

        if (info.size_ != length)
        {
#ifdef DEBUG
            std::cerr << "Exiv2::PngChunk::readRawProfile: Unable To Copy Raw Profile: cannot allocate memory\n";
#endif
            return DataBuf();
        }

        // Copy profile, skipping white space and column 1 "=" signs

        dp      = (unsigned char*)info.pData_;
        nibbles = length * 2;

        for (i = 0; i < (long) nibbles; i++)
        {
            while (*sp < '0' || (*sp > '9' && *sp < 'a') || *sp > 'f')
            {
                if (*sp == '\0')
                {
#ifdef DEBUG
                    std::cerr << "Exiv2::PngChunk::readRawProfile: Unable To Copy Raw Profile: ran out of data\n";
#endif
                    return DataBuf();
                }

                sp++;
            }

            if (i%2 == 0)
                *dp = (unsigned char) (16*unhex[(int) *sp++]);
            else
                (*dp++) += unhex[(int) *sp++];
        }

        return info;

    } // PngChunk::readRawProfile