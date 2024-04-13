gx_dc_pattern2_clip_with_bbox(const gx_device_color * pdevc, gx_device * pdev,
                              gx_clip_path *cpath_local, const gx_clip_path **ppcpath1)
{
    if (gx_dc_is_pattern2_color(pdevc) && gx_dc_pattern2_color_has_bbox(pdevc) &&
            (*dev_proc(pdev, dev_spec_op))(pdev, gxdso_pattern_shading_area, NULL, 0) == 0) {
        gs_pattern2_instance_t *pinst = (gs_pattern2_instance_t *)pdevc->ccolor.pattern;
        gx_path box_path;
        gs_memory_t *mem = (*ppcpath1 != NULL ? (*ppcpath1)->path.memory : pdev->memory);
        int code;

        gx_path_init_local(&box_path, mem);
        code = gx_dc_shading_path_add_box(&box_path, pdevc);
        if (code != gs_error_limitcheck) {
            /* Ignore huge BBox causing limitcheck - bug 689027. */
            if (code >= 0) {
                gx_cpath_init_local_shared(cpath_local, *ppcpath1, mem);
                code = gx_cpath_intersect(cpath_local, &box_path, gx_rule_winding_number, (gs_gstate *)pinst->saved);
                if (code < 0) {
                    gx_path_free(&box_path, "gx_default_fill_path(path_bbox)");
                    return code;
                }
                *ppcpath1 = cpath_local;
            }
        }
        gx_path_free(&box_path, "gx_default_fill_path(path_bbox)");
    }
    return 0;
}