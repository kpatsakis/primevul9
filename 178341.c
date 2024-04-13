jbig2_image_get_pixel(Jbig2Image *image, int x, int y)
{
    const int w = image->width;
    const int h = image->height;
    const int byte = (x >> 3) + y * image->stride;
    const int bit = 7 - (x & 7);

    if ((x < 0) || (x >= w))
        return 0;
    if ((y < 0) || (y >= h))
        return 0;

    return ((image->data[byte] >> bit) & 1);
}