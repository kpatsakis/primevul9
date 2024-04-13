pdf14_tile_pattern_fill(gx_device * pdev, const gs_gstate * pgs,
                        gx_path * ppath, const gx_fill_params * params,
                        const gx_device_color * pdevc,
                        const gx_clip_path * pcpath)
{
    int code;
    gs_gstate *pgs_noconst = (gs_gstate *)pgs; /* Break const. */
    gs_fixed_rect clip_box;
    gs_fixed_rect outer_box;
    pdf14_device * p14dev = (pdf14_device *)pdev;
    gs_int_rect rect;
    gx_clip_rect *curr_clip_rect;
    gx_color_tile *ptile = NULL;
    int k;
    gx_pattern_trans_t *fill_trans_buffer = NULL;
    gs_int_point phase;  /* Needed during clist rendering for band offset */
    int n_chan_tile;
    gx_clip_path cpath_intersection;
    gx_path path_ttrans;
    gs_blend_mode_t blend_mode;

    if (ppath == NULL)
        return_error(gs_error_unknownerror);	/* should not happen */
    if (pcpath != NULL) {
        code = gx_cpath_init_local_shared_nested(&cpath_intersection, pcpath, ppath->memory, 1);
    } else {
        (*dev_proc(pdev, get_clipping_box)) (pdev, &clip_box);
        gx_cpath_init_local(&cpath_intersection, ppath->memory);
        code = gx_cpath_from_rectangle(&cpath_intersection, &clip_box);
    }
    if (code < 0)
        return code;
    code = gx_cpath_intersect_with_params(&cpath_intersection, ppath,
                                          params->rule, pgs_noconst, params);
    if (code < 0)
        return code;
    /* One (common) case worth optimising for is where we have a pattern that
     * is positioned such that only one repeat of the tile is actually
     * visible. In this case, we can restrict the size of the blending group
     * we need to produce to be that of the actual area of the tile that is
     * used. */
    ptile = pdevc->colors.pattern.p_tile;
    if (ptile->ttrans != NULL)
    {
        if ((cpath_intersection.outer_box.p.x < 0) ||
            (cpath_intersection.outer_box.p.y < 0) ||
            (cpath_intersection.outer_box.q.x > int2fixed(ptile->ttrans->width)) ||
            (cpath_intersection.outer_box.q.y > int2fixed(ptile->ttrans->height)))
        {
            /* More than one repeat of the tile would be visible, so we can't
             * use the optimisation here. (Actually, this test isn't quite
             * right - it actually tests whether more than the '0th' repeat
             * of the tile is visible. A better test would test if just one
             * repeat of the tile was visible, irrespective of which one.
             * This is (hopefully) relatively rare, and would make the code
             * below more complex too, so we're ignoring that for now. If it
             * becomes evident that it's a case that matters we can revisit
             * it.) */
        } else {
            /* Only the 0th repeat is visible. Restrict the size further to
             * just the used area of that patch. */
            gx_path_init_local(&path_ttrans, ppath->memory);
            gx_path_add_rectangle(&path_ttrans,
                                  int2fixed(ptile->ttrans->rect.p.x),
                                  int2fixed(ptile->ttrans->rect.p.y),
                                  int2fixed(ptile->ttrans->rect.q.x),
                                  int2fixed(ptile->ttrans->rect.q.y));
            code = gx_cpath_intersect(&cpath_intersection, &path_ttrans,
                                      params->rule, pgs_noconst);
        }
    }
    /* Now let us push a transparency group into which we are
     * going to tile the pattern.  */
    if (ppath != NULL && code >= 0) {

        gx_cpath_outer_box(&cpath_intersection, &outer_box);
        rect.p.x = fixed2int(outer_box.p.x);
        rect.p.y = fixed2int(outer_box.p.y);
        rect.q.x = fixed2int_ceiling(outer_box.q.x);
        rect.q.y = fixed2int_ceiling(outer_box.q.y);

        /* The color space of this group must be the same as that of the
           tile.  Then when we pop the group, if there is a mismatch between
           the tile color space and the current context we will do the proper
           conversion.  In this way, we ensure that if the tile has any overlapping
           occuring it will be blended in the proper manner i.e in the tile
           underlying color space. */
        if (ptile->cdev == NULL) {
            if (ptile->ttrans == NULL)
                return_error(gs_error_unknownerror);	/* should not happen */
            n_chan_tile = ptile->ttrans->n_chan;
        } else {
            n_chan_tile = ptile->cdev->common.color_info.num_components+1;
        }
        blend_mode = ptile->blending_mode;
        code = pdf14_push_transparency_group(p14dev->ctx, &rect, 1, 0, 255,255,
                                             blend_mode, 0, 0, n_chan_tile-1,
                                             false, NULL, NULL, pgs_noconst,
                                             pdev);
        if (code < 0)
            return code;

        /* Set the blending procs and the is_additive setting based
           upon the number of channels */
        if (ptile->cdev == NULL) {
            if (n_chan_tile-1 < 4) {
                ptile->ttrans->blending_procs = &rgb_blending_procs;
                ptile->ttrans->is_additive = true;
            } else {
                ptile->ttrans->blending_procs = &cmyk_blending_procs;
                ptile->ttrans->is_additive = false;
            }
        }
        /* Now lets go through the rect list and fill with the pattern */
        /* First get the buffer that we will be filling */
        if (ptile->cdev == NULL) {
            fill_trans_buffer = new_pattern_trans_buff(pgs->memory);
            pdf14_get_buffer_information(pdev, fill_trans_buffer, NULL, false);
            /* Based upon if the tiles overlap pick the type of rect fill that we will
               want to use */
            if (ptile->has_overlap) {
                /* This one does blending since there is tile overlap */
                ptile->ttrans->pat_trans_fill = &tile_rect_trans_blend;
            } else {
                /* This one does no blending since there is no tile overlap */
                ptile->ttrans->pat_trans_fill = &tile_rect_trans_simple;
            }
            /* fill the rectangles */
            phase.x = pdevc->phase.x;
            phase.y = pdevc->phase.y;
            if (cpath_intersection.rect_list->list.head != NULL){
                curr_clip_rect = cpath_intersection.rect_list->list.head->next;
                for( k = 0; k< cpath_intersection.rect_list->list.count; k++){
                    if_debug5m('v', pgs->memory,
                               "[v]pdf14_tile_pattern_fill, (%d, %d), %d x %d pat_id %d \n",
                               curr_clip_rect->xmin, curr_clip_rect->ymin,
                               curr_clip_rect->xmax-curr_clip_rect->xmin,
                               curr_clip_rect->ymax-curr_clip_rect->ymin, (int)ptile->id);
                    gx_trans_pattern_fill_rect(curr_clip_rect->xmin, curr_clip_rect->ymin,
                                curr_clip_rect->xmax, curr_clip_rect->ymax, ptile,
                                fill_trans_buffer, phase, pdev, pdevc);
                    curr_clip_rect = curr_clip_rect->next;
                }
            } else if (cpath_intersection.rect_list->list.count == 1) {
                /* The case when there is just a single rect */
                if_debug5m('v', pgs->memory,
                           "[v]pdf14_tile_pattern_fill, (%d, %d), %d x %d pat_id %d \n",
                           cpath_intersection.rect_list->list.single.xmin,
                           cpath_intersection.rect_list->list.single.ymin,
                           cpath_intersection.rect_list->list.single.xmax-
                              cpath_intersection.rect_list->list.single.xmin,
                           cpath_intersection.rect_list->list.single.ymax-
                              cpath_intersection.rect_list->list.single.ymin,
                           (int)ptile->id);
                gx_trans_pattern_fill_rect(cpath_intersection.rect_list->list.single.xmin,
                                                cpath_intersection.rect_list->list.single.ymin,
                                                cpath_intersection.rect_list->list.single.xmax,
                                                cpath_intersection.rect_list->list.single.ymax,
                                                ptile, fill_trans_buffer, phase, pdev, pdevc);
            }
        } else {
            /* Clist pattern with transparency.  Create a clip device from our
               cpath_intersection.  The above non-clist case could probably be
               done this way too, which will reduce the amount of code here.
               That is for another day though due to time constraints*/
            gx_device *dev;
            gx_device_clip clipdev;

            gx_make_clip_device_on_stack(&clipdev, &cpath_intersection, pdev);
            dev = (gx_device *)&clipdev;
            phase.x = pdevc->phase.x;
            phase.y = pdevc->phase.y;
            gx_trans_pattern_fill_rect(rect.p.x, rect.p.y, rect.q.x, rect.q.y,
                                            ptile, fill_trans_buffer, phase,
                                            dev, pdevc);
        }
        /* free our buffer object */
        if (fill_trans_buffer != NULL) {
            gs_free_object(pgs->memory, fill_trans_buffer, "pdf14_tile_pattern_fill");
            ptile->ttrans->fill_trans_buffer = NULL;  /* Avoid GC issues */
        }
        /* pop our transparency group which will force the blending.
           This was all needed for Bug 693498 */
        code = pdf14_pop_transparency_group(pgs_noconst, p14dev->ctx,
                                            p14dev->blend_procs,
                                            p14dev->color_info.num_components,
                                            p14dev->icc_struct->device_profile[0],
                                            pdev);
    }
    return code;
}