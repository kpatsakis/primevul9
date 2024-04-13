void bjc_rgb_to_cmy(byte r, byte g, byte b,
                     int *c, int *m, int *y)
{   *c=255-r;
    *m=255-g;
    *y=255-b;
}