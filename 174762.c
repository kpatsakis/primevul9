    std::string binaryToHex(const byte *data, size_t size)
    {
        std::stringstream hexOutput;

        unsigned long tl = (unsigned long)((size / 16) * 16);
        unsigned long tl_offset = (unsigned long)(size - tl);

        for (unsigned long loop = 0; loop < (unsigned long)size; loop++) {
            if (data[loop] < 16) {
                hexOutput << "0";
            }
            hexOutput << std::hex << (int)data[loop];
            if ((loop % 8) == 7) {
                hexOutput << "  ";
            }
            if ((loop % 16) == 15 || loop == (tl + tl_offset - 1)) {
                int max = 15;
                if (loop >= tl) {
                    max = tl_offset - 1;
                    for (int offset = 0; offset < (int)(16 - tl_offset); offset++) {
                        if ((offset % 8) == 7) {
                            hexOutput << "  ";
                        }
                        hexOutput << "   ";
                    }
                }
                hexOutput << " ";
                for (int  offset = max; offset >= 0; offset--) {
                    if (offset == (max - 8)) {
                        hexOutput << "  ";
                    }
                    byte c = '.';
                    if (data[loop - offset] >= 0x20 && data[loop - offset] <= 0x7E) {
                        c = data[loop - offset] ;
                    }
                    hexOutput << (char) c ;
                }
                hexOutput << std::endl;
            }
        }

        hexOutput << std::endl << std::endl << std::endl;

        return hexOutput.str();
    }