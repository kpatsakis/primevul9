int epo_draw_line(gx_device *dev, int x0, int y0, int x1, int y1, gx_color_index color)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, obsolete_draw_line)(dev, x0, y0, x1, y1, color);
}