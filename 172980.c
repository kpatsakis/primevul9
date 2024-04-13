pdf14_text_begin(gx_device * dev, gs_gstate * pgs,
                 const gs_text_params_t * text, gs_font * font,
                 gx_path * path, const gx_device_color * pdcolor,
                 const gx_clip_path * pcpath, gs_memory_t * memory,
                 gs_text_enum_t ** ppenum)
{
    int code;
    gs_text_enum_t *penum;

    if_debug0m('v', memory, "[v]pdf14_text_begin\n");
    pdf14_set_marking_params(dev, pgs);
    code = gx_default_text_begin(dev, pgs, text, font, path, pdcolor, pcpath,
                                 memory, &penum);
    if (code < 0)
        return code;
    *ppenum = (gs_text_enum_t *)penum;
    return code;
}