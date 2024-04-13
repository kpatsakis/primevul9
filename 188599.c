void JBIG2Bitmap::duplicateRow(int yDest, int ySrc)
{
    memcpy(data + yDest * line, data + ySrc * line, line);
}