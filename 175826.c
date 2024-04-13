int epo_fill_linear_color_scanline(gx_device *dev, const gs_fill_attributes *fa,
        int i, int j, int w, const frac31 *c0, const int32_t *c0_f, const int32_t *cg_num,
        int32_t cg_den)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, fill_linear_color_scanline)(dev, fa, i, j, w, c0, c0_f, cg_num, cg_den);
}