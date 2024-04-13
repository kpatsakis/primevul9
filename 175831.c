int epo_strip_tile_rectangle(gx_device *dev, const gx_strip_bitmap *tiles, int x, int y, int width, int height,
    gx_color_index color0, gx_color_index color1,
    int phase_x, int phase_y)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, strip_tile_rectangle)(dev, tiles, x, y, width, height, color0, color1, phase_x, phase_y);
}