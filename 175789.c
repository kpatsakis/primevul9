int epo_fill_linear_color_triangle(gx_device *dev, const gs_fill_attributes *fa,
        const gs_fixed_point *p0, const gs_fixed_point *p1,
        const gs_fixed_point *p2, const frac31 *c0, const frac31 *c1, const frac31 *c2)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, fill_linear_color_triangle)(dev, fa, p0, p1, p2, c0, c1, c2);
}