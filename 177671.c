    static bool isStringType(uint16_t type)
    {
        return type == Exiv2::asciiString
            || type == Exiv2::unsignedByte
            || type == Exiv2::signedByte
            ;
    }