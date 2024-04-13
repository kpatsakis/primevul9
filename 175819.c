int epo_begin_image(gx_device *dev, const gs_gstate *pgs, const gs_image_t *pim,
    gs_image_format_t format, const gs_int_rect *prect,
    const gx_drawing_color *pdcolor, const gx_clip_path *pcpath,
    gs_memory_t *memory, gx_image_enum_common_t **pinfo)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, begin_image)(dev, pgs, pim, format, prect, pdcolor, pcpath, memory, pinfo);
}