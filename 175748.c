int epo_copy_color(gx_device *dev, const byte *data, int data_x, int raster, gx_bitmap_id id,\
    int x, int y, int width, int height)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, copy_color)(dev, data, data_x, raster, id, x, y, width, height);
}