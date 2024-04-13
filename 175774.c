int epo_fill_rectangle_hl_color(gx_device *dev, const gs_fixed_rect *rect,
        const gs_gstate *pgs, const gx_drawing_color *pdcolor, const gx_clip_path *pcpath)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, fill_rectangle_hl_color)(dev, rect, pgs, pdcolor, pcpath);
}