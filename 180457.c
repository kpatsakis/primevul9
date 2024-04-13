gx_default_fill_path(gx_device * pdev, const gs_gstate * pgs,
                     gx_path * ppath, const gx_fill_params * params,
                 const gx_device_color * pdevc, const gx_clip_path * pcpath)
{
    int code = 0;

    if (gx_dc_is_pattern2_color(pdevc)
        || pdevc->type == &gx_dc_type_data_ht_colored
        || (gx_dc_is_pattern1_color(pdevc) && gx_pattern_tile_is_clist(pdevc->colors.pattern.p_tile))
        ) {
        /*  Optimization for shading and halftone fill :
            The general filling algorithm subdivides the fill region into
            trapezoid or rectangle subregions and then paints each subregion
            with given color. If the color is complex, it also needs to be subdivided
            into constant color rectangles. In the worst case it gives
            a multiple of numbers of rectangles, which may be too slow.
            A faster processing may be obtained with installing a clipper
            device with the filling path, and then render the complex color
            through it. The speeding up happens due to the clipper device
            is optimised for fast scans through neighbour clipping rectangles.
        */
        /*  We need a single clipping path here, because shadings and
            halftones don't take 2 paths. Compute the clipping path intersection.
        */
        gx_clip_path cpath_intersection, cpath_with_shading_bbox;
        const gx_clip_path *pcpath1, *pcpath2;
        gs_gstate *pgs_noconst = (gs_gstate *)pgs; /* Break const. */

        if (ppath != NULL) {
            code = gx_cpath_init_local_shared_nested(&cpath_intersection, pcpath, pdev->memory, 1);
            if (code < 0)
                return code;
            if (pcpath == NULL) {
                gs_fixed_rect clip_box1;

                (*dev_proc(pdev, get_clipping_box)) (pdev, &clip_box1);
                code = gx_cpath_from_rectangle(&cpath_intersection, &clip_box1);
            }
            if (code >= 0)
                code = gx_cpath_intersect_with_params(&cpath_intersection, ppath, params->rule,
                        pgs_noconst, params);
            pcpath1 = &cpath_intersection;
        } else
            pcpath1 = pcpath;
        pcpath2 = pcpath1;
        if (code >= 0)
            code = gx_dc_pattern2_clip_with_bbox(pdevc, pdev, &cpath_with_shading_bbox, &pcpath1);
        /* Do fill : */
        if (code >= 0) {
            gs_fixed_rect clip_box;
            gs_int_rect cb;
            const gx_rop_source_t *rs = NULL;
            gx_device *dev;
            gx_device_clip cdev;

            gx_cpath_outer_box(pcpath1, &clip_box);
            cb.p.x = fixed2int_pixround(clip_box.p.x);
            cb.p.y = fixed2int_pixround(clip_box.p.y);
            cb.q.x = fixed2int_pixround(clip_box.q.x);
            cb.q.y = fixed2int_pixround(clip_box.q.y);
            if (gx_dc_is_pattern2_color(pdevc) &&
                    (*dev_proc(pdev, dev_spec_op))(pdev,
                         gxdso_pattern_handles_clip_path, NULL, 0) > 0) {
                /* A special interaction with clist writer device :
                   pass the intersected clipping path. It uses an unusual call to
                   fill_path with NULL device color. */
                code = (*dev_proc(pdev, fill_path))(pdev, pgs, ppath, params, NULL, pcpath1);
                dev = pdev;
            } else {
                gx_make_clip_device_on_stack(&cdev, pcpath1, pdev);
                dev = (gx_device *)&cdev;
                if ((*dev_proc(pdev, dev_spec_op))(pdev,
                        gxdso_pattern_shading_area, NULL, 0) > 0)
                    set_dev_proc(&cdev, fill_path, pass_shading_area_through_clip_path_device);
                code = 0;
            }
            if (code >= 0)
                code = pdevc->type->fill_rectangle(pdevc,
                        cb.p.x, cb.p.y, cb.q.x - cb.p.x, cb.q.y - cb.p.y,
                        dev, pgs->log_op, rs);
        }
        if (ppath != NULL)
            gx_cpath_free(&cpath_intersection, "shading_fill_cpath_intersection");
        if (pcpath1 != pcpath2)
            gx_cpath_free(&cpath_with_shading_bbox, "shading_fill_cpath_intersection");
    } else {
        code = gx_general_fill_path(pdev, pgs, ppath, params, pdevc, pcpath);
    }
    return code;
}