int epo_begin_typed_image(gx_device *dev, const gs_gstate *pgs, const gs_matrix *pmat,
    const gs_image_common_t *pic, const gs_int_rect *prect,
    const gx_drawing_color *pdcolor, const gx_clip_path *pcpath,
    gs_memory_t *memory, gx_image_enum_common_t **pinfo)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, begin_typed_image)(dev, pgs, pmat, pic, prect, pdcolor, pcpath, memory, pinfo);
}