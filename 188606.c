void JBIG2Bitmap::clearToZero()
{
    memset(data, 0, h * line);
}