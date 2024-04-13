gx_dc_pattern2_clip_with_bbox_simple(const gx_device_color * pdevc, gx_device * pdev,
                              gx_clip_path *cpath_local)
{
    int code = 0;

    if (gx_dc_is_pattern2_color(pdevc) && gx_dc_pattern2_color_has_bbox(pdevc) &&
            (*dev_proc(pdev, dev_spec_op))(pdev, gxdso_pattern_shading_area, NULL, 0) == 0) {
        gs_pattern2_instance_t *pinst = (gs_pattern2_instance_t *)pdevc->ccolor.pattern;
        gx_path box_path;
        gs_memory_t *mem = cpath_local->path.memory;

        gx_path_init_local(&box_path, mem);
        code = gx_dc_shading_path_add_box(&box_path, pdevc);
        if (code == gs_error_limitcheck) {
            /* Ignore huge BBox - bug 689027. */
            code = 0;
        } else if (code >= 0) {
            code = gx_cpath_intersect(cpath_local, &box_path, gx_rule_winding_number, (gs_gstate *)pinst->saved);
        }
        gx_path_free(&box_path, "gx_default_fill_path(path_bbox)");
    }
    return code;
}