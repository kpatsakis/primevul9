    const char *IptcData::detectCharset() const
    {
        const_iterator pos = findKey(IptcKey("Iptc.Envelope.CharacterSet"));
        if (pos != end()) {
            const std::string value = pos->toString();
            if (pos->value().ok()) {
                if (value == "\033%G") return "UTF-8";
                // other values are probably not practically relevant
            }
        }

        bool ascii = true;
        bool utf8 = true;

        for (pos = begin(); pos != end(); ++pos) {
            std::string value = pos->toString();
            if (pos->value().ok()) {
                int seqCount = 0;
                std::string::iterator i;
                for (i = value.begin(); i != value.end(); ++i) {
                    char c = *i;
                    if (seqCount) {
                        if ((c & 0xc0) != 0x80) {
                            utf8 = false;
                            break;
                        }
                        --seqCount;
                    }
                    else {
                        if (c & 0x80) ascii = false;
                        else continue; // ascii character

                        if      ((c & 0xe0) == 0xc0) seqCount = 1;
                        else if ((c & 0xf0) == 0xe0) seqCount = 2;
                        else if ((c & 0xf8) == 0xf0) seqCount = 3;
                        else if ((c & 0xfc) == 0xf8) seqCount = 4;
                        else if ((c & 0xfe) == 0xfc) seqCount = 5;
                        else {
                            utf8 = false;
                            break;
                        }
                    }
                }
                if (seqCount) utf8 = false; // unterminated seq
                if (!utf8) break;
            }
        }

        if (ascii) return "ASCII";
        if (utf8) return "UTF-8";
        return NULL;
    }