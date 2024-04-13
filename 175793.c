int epo_copy_mono(gx_device *dev, const byte *data, int data_x, int raster, gx_bitmap_id id,
    int x, int y, int width, int height,
    gx_color_index color0, gx_color_index color1)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, copy_mono)(dev, data, data_x, raster, id, x, y, width, height, color0, color1);
}