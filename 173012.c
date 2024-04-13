pdf14_patt_trans_image_fill(gx_device * dev, const gs_gstate * pgs,
                           const gs_matrix *pmat, const gs_image_common_t *pic,
                           const gs_int_rect * prect,
                           const gx_drawing_color * pdcolor,
                           const gx_clip_path * pcpath, gs_memory_t * mem,
                           gx_image_enum_common_t ** pinfo)
{
    const gs_image_t *pim = (const gs_image_t *)pic;
    pdf14_device * p14dev = (pdf14_device *)dev;
    gx_color_tile *ptile;
    int code;
    gs_int_rect group_rect;
    gx_image_enum *penum;
    gs_rect bbox_in, bbox_out;
    gx_pattern_trans_t *fill_trans_buffer;

    ptile = pdcolor->colors.pattern.p_tile;
    /* Set up things in the ptile so that we get the proper
       blending etc */
    /* Set the blending procs and the is_additive setting based
       upon the number of channels */
    if (ptile->ttrans->n_chan-1 < 4) {
        ptile->ttrans->blending_procs = &rgb_blending_procs;
        ptile->ttrans->is_additive = true;
    } else {
        ptile->ttrans->blending_procs = &cmyk_blending_procs;
        ptile->ttrans->is_additive = false;
    }
    /* Set the blending mode in the ptile based upon the current
       setting in the gs_gstate */
    ptile->blending_mode = pgs->blend_mode;
    /* Based upon if the tiles overlap pick the type of rect
       fill that we will want to use */
    if (ptile->has_overlap) {
        /* This one does blending since there is tile overlap */
        ptile->ttrans->pat_trans_fill = &tile_rect_trans_blend;
    } else {
        /* This one does no blending since there is no tile overlap */
        ptile->ttrans->pat_trans_fill = &tile_rect_trans_simple;
    }
    /* Set the procs so that we use the proper filling method. */
    gx_set_pattern_procs_trans((gx_device_color*) pdcolor);
    /* Let the imaging stuff get set up */
    code = gx_default_begin_typed_image(dev, pgs, pmat, pic,
                            prect, pdcolor,pcpath, mem, pinfo);
    if (code < 0)
        return code;
    /* Now Push the group */
    /* First apply the inverse of the image matrix to our
       image size to get our bounding box. */
    bbox_in.p.x = 0;
    bbox_in.p.y = 0;
    bbox_in.q.x = pim->Width;
    bbox_in.q.y = pim->Height;
    code = gs_bbox_transform_inverse(&bbox_in, &(pim->ImageMatrix),
                                &bbox_out);
    if (code < 0)
        return code;
    /* That in turn will get hit by the matrix in the gs_gstate */
    code = compute_group_device_int_rect(p14dev, &group_rect,
                                            &bbox_out, (gs_gstate *)pgs);
    if (code < 0)
        return code;
    if (!(pim->Width == 0 || pim->Height == 0)) {
        if_debug2m('?', p14dev->ctx->memory,
                   "[v*] Pushing trans group patt_trans_image_fill, uid = %ld id = %ld \n",
                   ptile->uid.id, ptile->id);
        code = pdf14_push_transparency_group(p14dev->ctx, &group_rect, 1, 0, 255,255,
                                             pgs->blend_mode, 0, 0,
                                             ptile->ttrans->n_chan-1, false, NULL,
                                             NULL, (gs_gstate *)pgs, dev);
        /* Set up the output buffer information now that we have
           pushed the group */
        fill_trans_buffer = new_pattern_trans_buff(pgs->memory);
        pdf14_get_buffer_information(dev, fill_trans_buffer, NULL, false);
        /* Store this in the appropriate place in pdcolor.  This
           is released later in pdf14_pattern_trans_render when
           we are all done with the mask fill */
        ptile->ttrans->fill_trans_buffer = fill_trans_buffer;
        /* Change the renderer to handle this case so we can catch the
           end.  We will then pop the group and reset the pdcolor proc.
           Keep the base renderer also. */
        penum = (gx_image_enum *) *pinfo;
        ptile->ttrans->image_render = penum->render;
        penum->render = &pdf14_pattern_trans_render;
        ptile->trans_group_popped = false;
    }
    return code;
}