int epo_draw_thin_line(gx_device *dev, fixed fx0, fixed fy0, fixed fx1, fixed fy1,
    const gx_drawing_color *pdcolor, gs_logical_operation_t lop,
    fixed adjustx, fixed adjusty)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, draw_thin_line)(dev, fx0, fy0, fx1, fy1, pdcolor, lop, adjustx, adjusty);
}