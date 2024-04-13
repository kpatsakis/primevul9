string_convbase64_6x4_to_8x3 (const unsigned char *from, unsigned char *to)
{
    to[0] = from[0] << 2 | from[1] >> 4;
    to[1] = from[1] << 4 | from[2] >> 2;
    to[2] = ((from[2] << 6) & 0xc0) | from[3];
}