gx_dc_pattern2_fill_rectangle(const gx_device_color * pdevc, int x, int y,
                              int w, int h, gx_device * dev,
                              gs_logical_operation_t lop,
                              const gx_rop_source_t * source)
{
    if (dev_proc(dev, dev_spec_op)(dev, gxdso_pattern_is_cpath_accum, NULL, 0)) {
        /* Performing a conversion of imagemask into a clipping path.
           Fall back to the device procedure. */
        return dev_proc(dev, fill_rectangle)(dev, x, y, w, h, (gx_color_index)0/*any*/);
    } else {
        gs_fixed_rect rect;
        gs_pattern2_instance_t *pinst =
            (gs_pattern2_instance_t *)pdevc->ccolor.pattern;

        rect.p.x = int2fixed(x);
        rect.p.y = int2fixed(y);
        rect.q.x = int2fixed(x + w);
        rect.q.y = int2fixed(y + h);
        return gs_shading_do_fill_rectangle(pinst->templat.Shading, &rect, dev,
                                    (gs_gstate *)pinst->saved, !pinst->shfill);
    }
}