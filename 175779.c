int epo_copy_alpha(gx_device *dev, const byte *data, int data_x,
    int raster, gx_bitmap_id id, int x, int y, int width, int height,
    gx_color_index color, int depth)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, copy_alpha)(dev, data, data_x, raster, id, x, y, width, height, color, depth);
}