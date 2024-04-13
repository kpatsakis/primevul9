pdf14_push_transparency_group(pdf14_ctx	*ctx, gs_int_rect *rect, bool isolated,
                              bool knockout, byte alpha, byte shape,
                              gs_blend_mode_t blend_mode, bool idle, uint mask_id,
                              int numcomps, bool cm_back_drop,
                              cmm_profile_t *group_profile,
                              cmm_profile_t *tos_profile, gs_gstate *pgs,
                              gx_device *dev)
{
    pdf14_buf *tos = ctx->stack;
    pdf14_buf *buf, *backdrop;
    bool has_shape, has_tags;

    if_debug1m('v', ctx->memory,
               "[v]pdf14_push_transparency_group, idle = %d\n", idle);

    /* We are going to use the shape in the knockout computation.  If previous
       buffer has a shape or if this is a knockout then we will have a shape here */
    has_shape = tos->has_shape || tos->knockout;
   // has_shape = false;
    /* If previous buffer has tags, then add tags here */
    has_tags = tos->has_tags;

    /* If the group is NOT isolated we add in the alpha_g plane.  This enables
       recompositing to be performed ala art_pdf_recomposite_group_8 so that
       the backdrop is only included one time in the computation. */
    /* Order of buffer data is color data, followed by alpha channel, followed by
       shape (if present), then alpha_g (if present), then tags (if present) */
    buf = pdf14_buf_new(rect, has_tags, !isolated, has_shape, idle, numcomps + 1,
                        tos->num_spots, ctx->memory);
    if_debug4m('v', ctx->memory,
               "[v]base buf: %d x %d, %d color channels, %d planes\n",
               buf->rect.q.x, buf->rect.q.y, buf->n_chan, buf->n_planes);
    if (buf == NULL)
        return_error(gs_error_VMerror);
    buf->isolated = isolated;
    buf->knockout = knockout;
    buf->alpha = alpha;
    buf->shape = shape;
    buf->blend_mode = blend_mode;
    buf->mask_id = mask_id;
    buf->mask_stack = ctx->mask_stack; /* Save because the group rendering may
                                          set up another (nested) mask. */
    ctx->mask_stack = NULL; /* Clean the mask field for rendering this group.
                            See pdf14_pop_transparency_group how to handle it. */
    buf->saved = tos;
    ctx->stack = buf;
    if (buf->data == NULL)
        return 0;
    if (idle)
        return 0;
    backdrop = pdf14_find_backdrop_buf(ctx);
    if (backdrop == NULL) {
        memset(buf->data, 0, buf->planestride * (buf->n_chan +
                                                 (buf->has_shape ? 1 : 0) +
                                                 (buf->has_alpha_g ? 1 : 0) +
                                                 (buf->has_tags ? 1 : 0)));
    } else {
        if (!buf->knockout) {
            if (!cm_back_drop) {
                pdf14_preserve_backdrop(buf, tos, false);
            } else {
                /* We must have an non-isolated group with a mismatch in color spaces.
                   In this case, we can't just copy the buffer but must CM it */
                pdf14_preserve_backdrop_cm(buf, group_profile, tos, tos_profile,
                                           ctx->memory, pgs, dev, false);
            }
        }
    }

    /* If knockout, we have to maintain a copy of the backdrop in case we are
       drawing nonisolated groups on top of the knockout group. */
    if (buf->knockout) {
        buf->backdrop = gs_alloc_bytes(ctx->memory, buf->planestride * buf->n_chan,
                                        "pdf14_push_transparency_group");
        if (buf->backdrop == NULL) {
            gs_free_object(ctx->memory, buf->backdrop, "pdf14_push_transparency_group");
            return gs_throw(gs_error_VMerror, "Knockout backdrop allocation failed");
        }
        if (buf->isolated) {
            /* We will have opaque backdrop for non-isolated compositing */
            memset(buf->backdrop, 0, buf->planestride * buf->n_chan);
        } else {
            /* Save knockout backdrop for non-isolated compositing */
            /* Note that we need to drill down through the non-isolated groups in our
               stack and make sure that we are not embedded in another knockout group */
            pdf14_buf *check = tos;
            pdf14_buf *child = NULL;  /* Needed so we can get profile */
            cmm_profile_t *prev_knockout_profile;

            while (check != NULL) {
                if (check->isolated)
                    break;
                if (check->knockout) {
                    break;
                }
                child = check;
                check = check->saved;
            }
            /* Here we need to grab a back drop from a knockout parent group and
                potentially worry about color differences. */
            if (check == NULL) {
                prev_knockout_profile = tos_profile;
                check = tos;
            } else {
                if (child == NULL) {
                    prev_knockout_profile = tos_profile;
                } else {
                    prev_knockout_profile  = child->parent_color_info_procs->icc_profile;
                }
            }
            if (!cm_back_drop) {
                pdf14_preserve_backdrop(buf, check, false);
            } else {
                /* We must have an non-isolated group with a mismatch in color spaces.
                   In this case, we can't just copy the buffer but must CM it */
                pdf14_preserve_backdrop_cm(buf, group_profile, check,
                                           prev_knockout_profile, ctx->memory, pgs,
                                           dev, false);
            }
            memcpy(buf->backdrop, buf->data, buf->planestride * buf->n_chan);
        }
#if RAW_DUMP
        /* Dump the current buffer to see what we have. */
        dump_raw_buffer(ctx->stack->rect.q.y-ctx->stack->rect.p.y,
                    ctx->stack->rowstride, buf->n_chan,
                    ctx->stack->planestride, ctx->stack->rowstride,
                    "KnockoutBackDrop", buf->backdrop);
        global_index++;
#endif
    } else {
        buf->backdrop = NULL;
    }
#if RAW_DUMP
    /* Dump the current buffer to see what we have. */
    dump_raw_buffer(ctx->stack->rect.q.y-ctx->stack->rect.p.y,
                ctx->stack->rowstride, ctx->stack->n_planes,
                ctx->stack->planestride, ctx->stack->rowstride,
                "TransGroupPush", ctx->stack->data);
    global_index++;
#endif
    return 0;
}