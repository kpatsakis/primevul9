string_convbase64_8x3_to_6x4 (const char *from, char *to)
{
    unsigned char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz0123456789+/";

    to[0] = base64_table [ (from[0] & 0xfc) >> 2 ];
    to[1] = base64_table [ ((from[0] & 0x03) << 4) + ((from[1] & 0xf0) >> 4) ];
    to[2] = base64_table [ ((from[1] & 0x0f) << 2) + ((from[2] & 0xc0) >> 6) ];
    to[3] = base64_table [ from[2] & 0x3f ];
}