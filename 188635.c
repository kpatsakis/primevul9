void JBIG2Bitmap::clearToOne()
{
    memset(data, 0xff, h * line);
}