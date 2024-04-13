bjc_invert_bytes(byte *row, uint raster, bool inverse, byte lastmask)
{   bool ret=false;

    for(; raster > 1; row++, raster--) {
        if(!(inverse)) *row = ~(*row);
        if(*row) ret = true;
    }
    if(!(inverse)) *row ^= 0xff;
    *row &= lastmask;
    return ret;
}