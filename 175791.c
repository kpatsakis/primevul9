int epo_fill_parallelogram(gx_device *dev, fixed px, fixed py, fixed ax, fixed ay, fixed bx, fixed by,
    const gx_drawing_color *pdcolor, gs_logical_operation_t lop)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, fill_parallelogram)(dev, px, py, ax, ay, bx, by, pdcolor, lop);
}