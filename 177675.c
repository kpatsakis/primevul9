    static bool is4ByteType(uint16_t type)
    {
        return isLongType(type)
            || isRationalType(type)
            ;
    }