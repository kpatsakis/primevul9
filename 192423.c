static UTF32 unescape_unicode(const unsigned char *p)
{
    char b;
    UTF32 result = 0;
    b = digit_values[p[0]];
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    result = (result << 4) | (unsigned char)b;
    b = digit_values[p[1]];
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    result = (result << 4) | (unsigned char)b;
    b = digit_values[p[2]];
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    result = (result << 4) | (unsigned char)b;
    b = digit_values[p[3]];
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    result = (result << 4) | (unsigned char)b;
    return result;
}