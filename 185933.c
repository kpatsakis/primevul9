    std::string PngChunk::writeRawProfile(const std::string& profileData,
                                          const char*        profileType)
    {
        static byte hex[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

        std::ostringstream oss;
        oss << '\n' << profileType << '\n' << std::setw(8) << profileData.size();
        const char* sp = profileData.data();
        for (std::string::size_type i = 0; i < profileData.size(); ++i) {
            if (i % 36 == 0) oss << '\n';
            oss << hex[((*sp >> 4) & 0x0f)];
            oss << hex[((*sp++) & 0x0f)];
        }
        oss << '\n';
        return oss.str();

    } // PngChunk::writeRawProfile