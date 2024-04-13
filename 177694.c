    static uint16_t byteSwap(uint16_t value,bool bSwap)
    {
        uint16_t result = 0;
        result |= (value & 0x00FF) << 8;
        result |= (value & 0xFF00) >> 8;
        return bSwap ? result : value;
    }