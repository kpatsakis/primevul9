    static bool isPrintICC(uint16_t type, Exiv2::PrintStructureOption option)
    {
        return type == 0x8773 && option == kpsIccProfile;
    }