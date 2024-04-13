gx_general_fill_path(gx_device * pdev, const gs_gstate * pgs,
                     gx_path * ppath, const gx_fill_params * params,
                 const gx_device_color * pdevc, const gx_clip_path * pcpath)
{
    gs_fixed_point adjust;
    gs_logical_operation_t lop = pgs->log_op;
    gs_fixed_rect ibox, bbox, sbox;
    gx_device_clip cdev;
    gx_device *dev = pdev;
    gx_device *save_dev = dev;
    gx_path ffpath;
    gx_path *pfpath;
    int code;
    int max_fill_band = dev->max_fill_band;
#define NO_BAND_MASK ((fixed)(-1) << (sizeof(fixed) * 8 - 1))
    const bool is_character = params->adjust.x == -1; /* See gxgstate.h */
    bool fill_by_trapezoids;
    bool big_path = ppath->subpath_count > 50;
    fill_options fo;
    line_list lst;

    *(const fill_options **)&lst.fo = &fo; /* break 'const'. */
    /*
     * Compute the bounding box before we flatten the path.
     * This can save a lot of time if the path has curves.
     * If the path is neither fully within nor fully outside
     * the quick-check boxes, we could recompute the bounding box
     * and make the checks again after flattening the path,
     * but right now we don't bother.
     */
    gx_path_bbox(ppath, &ibox);
    if (is_character)
        adjust.x = adjust.y = 0;
    else
        adjust = params->adjust;
    lst.contour_count = 0;
    lst.windings = NULL;
    lst.bbox_left = fixed2int(ibox.p.x - adjust.x - fixed_epsilon);
    lst.bbox_width = fixed2int(fixed_ceiling(ibox.q.x + adjust.x)) - lst.bbox_left;
    /* Check the bounding boxes. */
    if_debug6m('f', pdev->memory, "[f]adjust=%g,%g bbox=(%g,%g),(%g,%g)\n",
               fixed2float(adjust.x), fixed2float(adjust.y),
               fixed2float(ibox.p.x), fixed2float(ibox.p.y),
               fixed2float(ibox.q.x), fixed2float(ibox.q.y));
    if (pcpath)
        gx_cpath_inner_box(pcpath, &bbox);
    else
        (*dev_proc(dev, get_clipping_box)) (dev, &bbox);
    if (!rect_within(ibox, bbox)) {
        /*
         * Intersect the path box and the clip bounding box.
         * If the intersection is empty, this fill is a no-op.
         */
        if (pcpath)
            gx_cpath_outer_box(pcpath, &bbox);
        if_debug4m('f', pdev->memory, "   outer_box=(%g,%g),(%g,%g)\n",
                   fixed2float(bbox.p.x), fixed2float(bbox.p.y),
                   fixed2float(bbox.q.x), fixed2float(bbox.q.y));
        rect_intersect(ibox, bbox);
        if (ibox.p.x - adjust.x >= ibox.q.x + adjust.x ||
            ibox.p.y - adjust.y >= ibox.q.y + adjust.y
            ) {                 /* Intersection of boxes is empty! */
            return 0;
        }
        /*
         * The path is neither entirely inside the inner clip box
         * nor entirely outside the outer clip box.
         * If we had to flatten the path, this is where we would
         * recompute its bbox and make the tests again,
         * but we don't bother right now.
         *
         * If there is a clipping path, set up a clipping device.
         */
        if (pcpath) {
            dev = (gx_device *) & cdev;
            gx_make_clip_device_on_stack(&cdev, pcpath, save_dev);
            cdev.max_fill_band = save_dev->max_fill_band;
        }
    }
    /*
     * Compute the proper adjustment values.
     * To get the effect of the any-part-of-pixel rule,
     * we may have to tweak them slightly.
     * NOTE: We changed the adjust_right/above value from 0.5+epsilon
     * to 0.5 in release 5.01; even though this does the right thing
     * in every case we could imagine, we aren't confident that it's
     * correct.  (The old values were definitely incorrect, since they
     * caused 1-pixel-wide/high objects to color 2 pixels even if
     * they fell exactly on pixel boundaries.)
     */
    if (adjust.x == fixed_half)
        fo.adjust_left = fixed_half - fixed_epsilon,
            fo.adjust_right = fixed_half /* + fixed_epsilon */ ;        /* see above */
    else
        fo.adjust_left = fo.adjust_right = adjust.x;
    if (adjust.y == fixed_half)
        fo.adjust_below = fixed_half - fixed_epsilon,
            fo.adjust_above = fixed_half /* + fixed_epsilon */ ;        /* see above */
    else
        fo.adjust_below = fo.adjust_above = adjust.y;
    /* Initialize the active line list. */
    init_line_list(&lst, ppath->memory);
    sbox.p.x = ibox.p.x - adjust.x;
    sbox.p.y = ibox.p.y - adjust.y;
    sbox.q.x = ibox.q.x + adjust.x;
    sbox.q.y = ibox.q.y + adjust.y;
    fo.pdevc = pdevc;
    fo.lop = lop;
    fo.fixed_flat = float2fixed(params->flatness);
    fo.ymin = ibox.p.y;
    fo.ymax = ibox.q.y;
    fo.dev = dev;
    fo.pbox = &sbox;
    fo.rule = params->rule;
    fo.is_spotan = is_spotan_device(dev);
    fo.fill_direct = color_writes_pure(pdevc, lop);
    fo.fill_rect = (fo.fill_direct ? dev_proc(dev, fill_rectangle) : NULL);
    fo.fill_trap = dev_proc(dev, fill_trapezoid);

    /*
     * We have a choice of two different filling algorithms:
     * scan-line-based and trapezoid-based.  They compare as follows:
     *
     *      Scan    Trap
     *      ----    ----
     *       skip   +draw   0-height horizontal lines
     *       slow   +fast   rectangles
     *      +fast    slow   curves
     *      +yes     no     write pixels at most once when adjust != 0
     *
     * Normally we use the scan line algorithm for characters, where curve
     * speed is important, and for non-idempotent RasterOps, where double
     * pixel writing must be avoided, and the trapezoid algorithm otherwise.
     * However, we always use the trapezoid algorithm for rectangles.
     */
    fill_by_trapezoids = (!gx_path_has_curves(ppath) ||
                          params->flatness >= 1.0 || fo.is_spotan);

    if (fill_by_trapezoids && !fo.is_spotan && !lop_is_idempotent(lop)) {
        gs_fixed_rect rbox;

        if (gx_path_is_rectangular(ppath, &rbox)) {
            int x0 = fixed2int_pixround(rbox.p.x - fo.adjust_left);
            int y0 = fixed2int_pixround(rbox.p.y - fo.adjust_below);
            int x1 = fixed2int_pixround(rbox.q.x + fo.adjust_right);
            int y1 = fixed2int_pixround(rbox.q.y + fo.adjust_above);

            return gx_fill_rectangle_device_rop(x0, y0, x1 - x0, y1 - y0,
                                                pdevc, dev, lop);
        }
        if (fo.adjust_left | fo.adjust_right | fo.adjust_below | fo.adjust_above)
            fill_by_trapezoids = false; /* avoid double writing pixels */
    }

    if (!fo.is_spotan && gs_getscanconverter(pdev->memory) >= GS_SCANCONVERTER_EDGEBUFFER) {
        gx_edgebuffer eb = { 0 };
        if (fill_by_trapezoids && !lop_is_idempotent(lop))
            fill_by_trapezoids = 0;
        if (!fill_by_trapezoids)
        {
            if (adjust.x == 0 && adjust.y == 0) {
                code = gx_scan_convert(dev,
                                       ppath,
                                       &ibox,
                                       &eb,
                                       fo.fixed_flat);
                if (code >= 0) {
                    code = gx_filter_edgebuffer(dev,
                                                &eb,
                                                params->rule);
                }
                if (code >= 0) {
                    code = gx_fill_edgebuffer(dev,
                                              pdevc,
                                              &eb,
                                              fo.fill_direct ? -1 : (int)pgs->log_op);
                }
            } else {
                code = gx_scan_convert_app(dev,
                                           ppath,
                                           &ibox,
                                           &eb,
                                           fo.fixed_flat);
                if (code >= 0) {
                    code = gx_filter_edgebuffer_app(dev,
                                                    &eb,
                                                    params->rule);
                }
                if (code >= 0) {
                    code = gx_fill_edgebuffer_app(dev,
                                                  pdevc,
                                                  &eb,
                                                  fo.fill_direct ? -1 : (int)pgs->log_op);
                }
            }
        } else {
            if (adjust.x == 0 && adjust.y == 0) {
                code = gx_scan_convert_tr(dev,
                                          ppath,
                                          &ibox,
                                          &eb,
                                          fo.fixed_flat);
                if (code >= 0) {
                    code = gx_filter_edgebuffer_tr(dev,
                                                   &eb,
                                                    params->rule);
                }
                if (code >= 0) {
                    code = gx_fill_edgebuffer_tr(dev,
                                                 pdevc,
                                                 &eb,
                                                 (int)pgs->log_op);
                }
            } else {
                code = gx_scan_convert_tr_app(dev,
                                              ppath,
                                              &ibox,
                                              &eb,
                                              fo.fixed_flat);
                if (code >= 0) {
                    code = gx_filter_edgebuffer_tr_app(dev,
                                                       &eb,
                                                       params->rule);
                }
                if (code >= 0) {
                    code = gx_fill_edgebuffer_tr_app(dev,
                                                     pdevc,
                                                     &eb,
                                                     (int)pgs->log_op);
                }
            }
        }
        gx_edgebuffer_fin(dev,&eb);
        return code;
    }

    gx_path_init_local(&ffpath, ppath->memory);
    if (!big_path && !gx_path_has_curves(ppath))        /* don't need to flatten */
        pfpath = ppath;
    else if (is_spotan_device(dev))
        pfpath = ppath;
    else if (!big_path && !pgs->accurate_curves && gx_path__check_curves(ppath, pco_small_curves, fo.fixed_flat))
        pfpath = ppath;
    else {
        code = gx_path_copy_reducing(ppath, &ffpath, fo.fixed_flat, NULL,
                                     pco_small_curves | (pgs->accurate_curves ? pco_accurate : 0));
        if (code < 0)
            return code;
        pfpath = &ffpath;
        if (big_path) {
            code = gx_path_merge_contacting_contours(pfpath);
            if (code < 0)
                return code;
        }
    }
    fo.fill_by_trapezoids = fill_by_trapezoids;
    if ((code = add_y_list(pfpath, &lst)) < 0)
        goto nope;
    {
        FILL_LOOP_PROC((*fill_loop));

        /* Some short-sighted compilers won't allow a conditional here.... */
        if (fill_by_trapezoids)
            fill_loop = spot_into_trapezoids;
        else
            fill_loop = spot_into_scan_lines;
        if (gs_currentcpsimode(pgs->memory) && is_character) {
            if (lst.contour_count > countof(lst.local_windings)) {
                lst.windings = (int *)gs_alloc_byte_array(pdev->memory, lst.contour_count,
                                sizeof(int), "gx_general_fill_path");
            } else
                lst.windings = lst.local_windings;
            memset(lst.windings, 0, sizeof(lst.windings[0]) * lst.contour_count);
        }
        code = (*fill_loop)
            (&lst, (max_fill_band == 0 ? NO_BAND_MASK : int2fixed(-max_fill_band)));
        if (lst.windings != NULL && lst.windings != lst.local_windings)
            gs_free_object(pdev->memory, lst.windings, "gx_general_fill_path");
    }
  nope:if (lst.close_count != 0)
        unclose_path(pfpath, lst.close_count);
    free_line_list(&lst);
    if (pfpath != ppath)        /* had to flatten */
        gx_path_free(pfpath, "gx_general_fill_path");
#if defined(DEBUG) && !defined(GS_THREADSAFE)
    if (gs_debug_c('f')) {
        dmlputs(ppath->memory,
                "[f]  # alloc    up  down horiz step slowx  iter  find  band bstep bfill\n");
        dmlprintf5(ppath->memory, " %5ld %5ld %5ld %5ld %5ld",
                   stats_fill.fill, stats_fill.fill_alloc,
                   stats_fill.y_up, stats_fill.y_down,
                   stats_fill.horiz);
        dmlprintf4(ppath->memory, " %5ld %5ld %5ld %5ld",
                   stats_fill.x_step, stats_fill.slow_x,
                   stats_fill.iter, stats_fill.find_y);
        dmlprintf3(ppath->memory, " %5ld %5ld %5ld\n",
                   stats_fill.band, stats_fill.band_step,
                   stats_fill.band_fill);
        dmlputs(ppath->memory,
                "[f]    afill slant shall sfill mqcrs order slowo\n");
        dmlprintf7(ppath->memory, "       %5ld %5ld %5ld %5ld %5ld %5ld %5ld\n",
                   stats_fill.afill, stats_fill.slant,
                   stats_fill.slant_shallow, stats_fill.sfill,
                   stats_fill.mq_cross, stats_fill.order,
                   stats_fill.slow_order);
    }
#endif
    return code;
}