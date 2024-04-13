int epo_stroke_path(gx_device *dev, const gs_gstate *pgs, gx_path *ppath,
    const gx_stroke_params *params,
    const gx_drawing_color *pdcolor, const gx_clip_path *pcpath)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, stroke_path)(dev, pgs, ppath, params, pdcolor, pcpath);
}