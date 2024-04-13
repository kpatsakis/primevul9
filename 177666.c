    static uint32_t byteSwap(uint32_t value,bool bSwap)
    {
        uint32_t result = 0;
        result |= (value & 0x000000FF) << 24;
        result |= (value & 0x0000FF00) << 8;
        result |= (value & 0x00FF0000) >> 8;
        result |= (value & 0xFF000000) >> 24;
        return bSwap ? result : value;
    }