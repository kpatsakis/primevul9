    virtual UnicodeString& format(int32_t ,UnicodeString& appendTo,FieldPosition& ) const {
        return appendTo.append((UChar)0x0033);
    }