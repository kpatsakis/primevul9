compute_pixel_type (XwdLoader *loader)
{
    XwdHeader *h = &loader->header;

    if (h->bits_per_pixel == 24)
    {
        if (h->byte_order == 0)
            return CHAFA_PIXEL_BGR8;
        else
            return CHAFA_PIXEL_RGB8;
    }

    if (h->bits_per_pixel == 32)
    {
        if (h->byte_order == 0)
            return CHAFA_PIXEL_BGRA8_PREMULTIPLIED;
        else
            return CHAFA_PIXEL_ARGB8_PREMULTIPLIED;
    }

    return CHAFA_PIXEL_MAX;
}