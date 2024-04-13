int epo_strip_copy_rop2(gx_device *dev, const byte *sdata, int sourcex, uint sraster, gx_bitmap_id id,
    const gx_color_index *scolors, const gx_strip_bitmap *textures, const gx_color_index *tcolors,
    int x, int y, int width, int height, int phase_x, int phase_y, gs_logical_operation_t lop, uint planar_height)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, strip_copy_rop2)(dev, sdata, sourcex, sraster, id, scolors, textures, tcolors, x, y, width, height, phase_x, phase_y, lop, planar_height);
}