bjc_invert_cmyk_bytes(byte *rowC, byte *rowM, byte *rowY, byte *rowK,
                      uint raster, bool inverse, byte lastmask,
                     skip_t *skip)
{   bool ret=false;
    byte tmpC, tmpM, tmpY;

    skip->skipC=false;
    skip->skipM=false;
    skip->skipY=false;
    skip->skipK=false;

    for(; raster > 1; rowC++, rowM++, rowY++, rowK++, raster--) {
        if(inverse) {
                      tmpC = ~(*rowC|*rowK);
                      tmpM = ~(*rowM|*rowK);
                      tmpY = ~(*rowY|*rowK);
                     *rowK = ~(*rowC|*rowM|*rowY|*rowK);
                     *rowC = tmpC;
                     *rowM = tmpM;
                     *rowY = tmpY;
        }
        if(*rowC) skip->skipC=true;
        if(*rowM) skip->skipM=true;
        if(*rowY) skip->skipY=true;
        if(*rowK) skip->skipK=true;
        if(*rowC|*rowM|*rowY|*rowK) ret = true;
    }
    return ret;
}