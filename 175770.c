int epo_fill_trapezoid(gx_device *dev, const gs_fixed_edge *left, const gs_fixed_edge *right,
    fixed ybot, fixed ytop, bool swap_axes,
    const gx_drawing_color *pdcolor, gs_logical_operation_t lop)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, fill_trapezoid)(dev, left, right, ybot, ytop, swap_axes, pdcolor, lop);
}