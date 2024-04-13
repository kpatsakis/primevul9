int epo_copy_rop(gx_device *dev, const byte *sdata, int sourcex, uint sraster, gx_bitmap_id id,
    const gx_color_index *scolors,
    const gx_tile_bitmap *texture, const gx_color_index *tcolors,
    int x, int y, int width, int height,
    int phase_x, int phase_y, gs_logical_operation_t lop)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, copy_rop)(dev, sdata, sourcex, sraster, id, scolors, texture, tcolors, x, y, width, height, phase_x, phase_y, lop);
}