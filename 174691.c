xf86GetBppFromDepth(ScrnInfoPtr pScrn, int depth)
{
    PixmapFormatPtr format;

    format = xf86GetPixFormat(pScrn, depth);
    if (format)
        return format->bitsPerPixel;
    else
        return 0;
}