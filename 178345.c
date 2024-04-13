jbig2_image_set_pixel(Jbig2Image *image, int x, int y, bool value)
{
    const int w = image->width;
    const int h = image->height;
    int scratch, mask;
    int bit, byte;

    if ((x < 0) || (x >= w))
        return;
    if ((y < 0) || (y >= h))
        return;

    byte = (x >> 3) + y * image->stride;
    bit = 7 - (x & 7);
    mask = (1 << bit) ^ 0xff;

    scratch = image->data[byte] & mask;
    image->data[byte] = scratch | (value << bit);
}