NumberFormatTest::escape(UnicodeString& s)
{
    UnicodeString buf;
    for (int32_t i=0; i<s.length(); ++i)
    {
        UChar c = s[(int32_t)i];
        if (c <= (UChar)0x7F) buf += c;
        else {
            buf += (UChar)0x5c; buf += (UChar)0x55;
            buf += toHexString((c & 0xF000) >> 12);
            buf += toHexString((c & 0x0F00) >> 8);
            buf += toHexString((c & 0x00F0) >> 4);
            buf += toHexString(c & 0x000F);
        }
    }
    return (s = buf);
}