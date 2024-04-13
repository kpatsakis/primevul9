    virtual UnicodeString& format(int64_t number,UnicodeString& appendTo,FieldPosition& pos) const {
        return NumberFormat::format(number, appendTo, pos);
    }