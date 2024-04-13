    void Image::setXmpPacket(const std::string& xmpPacket)
    {
        xmpPacket_ = xmpPacket;
        if ( XmpParser::decode(xmpData_, xmpPacket) ) {
            throw Error(54);
        }
        xmpPacket_ = xmpPacket;
    }