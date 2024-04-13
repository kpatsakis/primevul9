    static const char* tagName(uint16_t tag,size_t nMaxLength)
    {
        const char* result = NULL;

        // build a static map of tags for fast search
        static std::map<int,std::string> tags;
        static bool init  = true;
        static char buffer[80];

        if ( init ) {
            int idx;
            const TagInfo* ti ;
            for (ti = Exiv2::  mnTagList(), idx = 0; ti[idx].tag_ != 0xffff; ++idx) tags[ti[idx].tag_] = ti[idx].name_;
            for (ti = Exiv2:: iopTagList(), idx = 0; ti[idx].tag_ != 0xffff; ++idx) tags[ti[idx].tag_] = ti[idx].name_;
            for (ti = Exiv2:: gpsTagList(), idx = 0; ti[idx].tag_ != 0xffff; ++idx) tags[ti[idx].tag_] = ti[idx].name_;
            for (ti = Exiv2:: ifdTagList(), idx = 0; ti[idx].tag_ != 0xffff; ++idx) tags[ti[idx].tag_] = ti[idx].name_;
            for (ti = Exiv2::exifTagList(), idx = 0; ti[idx].tag_ != 0xffff; ++idx) tags[ti[idx].tag_] = ti[idx].name_;
            for (ti = Exiv2:: mpfTagList(), idx = 0; ti[idx].tag_ != 0xffff; ++idx) tags[ti[idx].tag_] = ti[idx].name_;
        }
        init = false;

        try {
            result = tags[tag].c_str();
            if ( nMaxLength > sizeof(buffer) -2 )
                 nMaxLength = sizeof(buffer) -2;
            strncpy(buffer,result,nMaxLength);
            result = buffer;
        } catch ( ... ) {}

        return result ;
    }