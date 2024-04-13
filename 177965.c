void bjc_rgb_to_gray(byte r, byte g, byte b,
                     int *k)
{
    *k = ( (int)r * 77 + (int)g * 151 + (int)b * 28) >> 8;
}