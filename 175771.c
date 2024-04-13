int epo_fill_rectangle(gx_device *dev, int x, int y, int width, int height, gx_color_index color)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, fill_rectangle)(dev, x, y, width, height, color);
}