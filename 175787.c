int epo_text_begin(gx_device *dev, gs_gstate *pgs, const gs_text_params_t *text,
    gs_font *font, gx_path *path, const gx_device_color *pdcolor, const gx_clip_path *pcpath,
    gs_memory_t *memory, gs_text_enum_t **ppte)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, text_begin)(dev, pgs, text, font, path, pdcolor, pcpath, memory, ppte);
}