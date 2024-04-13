gx_dc_pattern2_is_rectangular_cell(const gx_device_color * pdevc, gx_device * pdev, gs_fixed_rect *rect)
{
    if (gx_dc_is_pattern2_color(pdevc) && gx_dc_pattern2_color_has_bbox(pdevc) &&
            (*dev_proc(pdev, dev_spec_op))(pdev, gxdso_pattern_shading_area, NULL, 0) == 0) {
        gs_pattern2_instance_t *pinst = (gs_pattern2_instance_t *)pdevc->ccolor.pattern;
        const gs_shading_t *psh = pinst->templat.Shading;
        gs_fixed_point p, q;

        if (is_xxyy(&ctm_only(pinst->saved)))
            if (psh->params.have_BBox) {
                int code = gs_point_transform2fixed(&pinst->saved->ctm,
                            psh->params.BBox.p.x, psh->params.BBox.p.y, &p);
                if (code < 0)
                    return code;
                code = gs_point_transform2fixed(&pinst->saved->ctm,
                            psh->params.BBox.q.x, psh->params.BBox.q.y, &q);
                if (code < 0)
                    return code;
                if (p.x > q.x) {
                    p.x ^= q.x; q.x ^= p.x; p.x ^= q.x;
                }
                if (p.y > q.y) {
                    p.y ^= q.y; q.y ^= p.y; p.y ^= q.y;
                }
                rect->p = p;
                rect->q = q;
                return 1;
            }
    }
    return 0;
}