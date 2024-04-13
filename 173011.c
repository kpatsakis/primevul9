pdf14_pop_transparency_mask(pdf14_ctx *ctx, gs_gstate *pgs, gx_device *dev)
{
    pdf14_buf *tos = ctx->stack;
    byte *new_data_buf;
    int icc_match;
    cmm_profile_t *des_profile = tos->parent_color_info_procs->icc_profile; /* If set, this should be a gray profile */
    cmm_profile_t *src_profile;
    gsicc_rendering_param_t rendering_params;
    gsicc_link_t *icc_link;
    gsicc_rendering_param_t render_cond;
    cmm_dev_profile_t *dev_profile;

    dev_proc(dev, get_profile)(dev,  &dev_profile);
    gsicc_extract_profile(GS_UNKNOWN_TAG, dev_profile, &src_profile,
                          &render_cond);
    ctx->smask_depth -= 1;
    /* icc_match == -1 means old non-icc code.
       icc_match == 0 means use icc code
       icc_match == 1 mean no conversion needed */
    if ( des_profile != NULL && src_profile != NULL ) {
        icc_match = (des_profile->hashcode ==  src_profile->hashcode);
    } else {
        icc_match = -1;
    }
    if_debug1m('v', ctx->memory, "[v]pdf14_pop_transparency_mask, idle=%d\n",
               tos->idle);
    ctx->stack = tos->saved;
    tos->saved = NULL;  /* To avoid issues with GC */
    if (tos->mask_stack) {
        /* During the soft mask push, the mask_stack was copied (not moved) from
           the ctx to the tos mask_stack. We are done with this now so it is safe to
           just set to NULL.  However, before we do that we must perform
           rc decrement to match the increment that occured was made.  Also,
           if this is the last ref count of the rc_mask, we should free the
           buffer now since no other groups need it. */
        rc_decrement(tos->mask_stack->rc_mask,
                     "pdf14_pop_transparency_mask(tos->mask_stack->rc_mask)");
        if (tos->mask_stack->rc_mask) {
            if (tos->mask_stack->rc_mask->rc.ref_count == 1){
                rc_decrement(tos->mask_stack->rc_mask,
                            "pdf14_pop_transparency_mask(tos->mask_stack->rc_mask)");
            }
        }
        tos->mask_stack = NULL;
    }
    if (tos->data == NULL ) {
        /* This can occur in clist rendering if the soft mask does
           not intersect the current band.  It would be nice to
           catch this earlier and just avoid creating the structure
           to begin with.  For now we need to delete the structure
           that was created.  Only delete if the alpha value is 255 */
        if (tos->alpha == 255) {
            pdf14_buf_free(tos, ctx->memory);
            if (ctx->mask_stack != NULL) {
                pdf14_free_mask_stack(ctx, ctx->memory);
            }
        } else {
            /* Assign as mask buffer */
            if (ctx->mask_stack != NULL) {
                pdf14_free_mask_stack(ctx, ctx->memory);
            }
            ctx->mask_stack = pdf14_mask_element_new(ctx->memory);
            ctx->mask_stack->rc_mask = pdf14_rcmask_new(ctx->memory);
            ctx->mask_stack->rc_mask->mask_buf = tos;
        }
        ctx->smask_blend = false;  /* just in case */
    } else {
        /* If we are already in the source space then there is no reason
           to do the transformation */
        /* Lets get this to a monochrome buffer and map it to a luminance only value */
        /* This will reduce our memory.  We won't reuse the existing one, due */
        /* Due to the fact that on certain systems we may have issues recovering */
        /* the data after a resize */
        new_data_buf = gs_alloc_bytes(ctx->memory, tos->planestride,
                                        "pdf14_pop_transparency_mask");
        if (new_data_buf == NULL)
            return_error(gs_error_VMerror);
        /* Initialize with 0.  Need to do this since in Smask_Luminosity_Mapping
           we won't be filling everything during the remap if it had not been
           written into by the PDF14 fill rect */
        memset(new_data_buf, 0, tos->planestride);
        /* If the subtype was alpha, then just grab the alpha channel now
           and we are all done */
        if (tos->SMask_SubType == TRANSPARENCY_MASK_Alpha) {
            ctx->smask_blend = false;  /* not used in this case */
            smask_copy(tos->rect.q.y - tos->rect.p.y,
                       tos->rect.q.x - tos->rect.p.x,
                       tos->rowstride,
                       (tos->data)+tos->planestride, new_data_buf);
#if RAW_DUMP
            /* Dump the current buffer to see what we have. */
            dump_raw_buffer(tos->rect.q.y-tos->rect.p.y,
                        tos->rowstride, tos->n_planes,
                        tos->planestride, tos->rowstride,
                        "SMask_Pop_Alpha(Mask_Plane1)",tos->data);
            global_index++;
#endif
        } else {
            if ( icc_match == 1 || tos->n_chan == 2) {
#if RAW_DUMP
                /* Dump the current buffer to see what we have. */
                dump_raw_buffer(tos->rect.q.y-tos->rect.p.y,
                            tos->rowstride, tos->n_planes,
                            tos->planestride, tos->rowstride,
                            "SMask_Pop_Lum(Mask_Plane0)",tos->data);
                global_index++;
#endif
                /* There is no need to color convert.  Data is already gray scale.
                   We just need to copy the gray plane.  However it is
                   possible that the soft mask could have a soft mask which
                   would end us up with some alpha blending information
                   (Bug691803). In fact, according to the spec, the alpha
                   blending has to occur.  See FTS test fts_26_2601.pdf
                   for an example of this.  Softmask buffer is intialized
                   with BG values.  It would be nice to keep track if buffer
                   ever has a alpha value not 1 so that we could detect and
                   avoid this blend if not needed. */
                smask_blend(tos->data, tos->rect.q.x - tos->rect.p.x,
                            tos->rect.q.y - tos->rect.p.y, tos->rowstride,
                            tos->planestride);
#if RAW_DUMP
                /* Dump the current buffer to see what we have. */
                dump_raw_buffer(tos->rect.q.y-tos->rect.p.y,
                            tos->rowstride, tos->n_planes,
                            tos->planestride, tos->rowstride,
                            "SMask_Pop_Lum_Post_Blend",tos->data);
                global_index++;
#endif
                smask_copy(tos->rect.q.y - tos->rect.p.y,
                           tos->rect.q.x - tos->rect.p.x,
                           tos->rowstride, tos->data, new_data_buf);
            } else {
                if ( icc_match == -1 ) {
                    /* The slow old fashioned way */
                    smask_luminosity_mapping(tos->rect.q.y - tos->rect.p.y ,
                        tos->rect.q.x - tos->rect.p.x,tos->n_chan,
                        tos->rowstride, tos->planestride,
                        tos->data,  new_data_buf, ctx->additive, tos->SMask_SubType);
                } else {
                    /* ICC case where we use the CMM */
                    /* Request the ICC link for the transform that we will need to use */
                    rendering_params.black_point_comp = gsBLACKPTCOMP_OFF;
                    rendering_params.graphics_type_tag = GS_IMAGE_TAG;
                    rendering_params.override_icc = false;
                    rendering_params.preserve_black = gsBKPRESNOTSPECIFIED;
                    rendering_params.rendering_intent = gsPERCEPTUAL;
                    rendering_params.cmm = gsCMM_DEFAULT;
                    icc_link = gsicc_get_link_profile(pgs, dev, des_profile,
                        src_profile, &rendering_params, pgs->memory, false);
                    smask_icc(dev, tos->rect.q.y - tos->rect.p.y,
                              tos->rect.q.x - tos->rect.p.x,tos->n_chan,
                              tos->rowstride, tos->planestride,
                              tos->data, new_data_buf, icc_link);
                    /* Release the link */
                    gsicc_release_link(icc_link);
                }
            }
        }
        /* Free the old object, NULL test was above */
        gs_free_object(ctx->memory, tos->data, "pdf14_pop_transparency_mask");
        tos->data = new_data_buf;
        /* Data is single channel now */
        tos->n_chan = 1;
        tos->n_planes = 1;
        /* Assign as reference counted mask buffer */
        if (ctx->mask_stack != NULL) {
            /* In this case, the source file is wacky as it already had a
               softmask and now is getting a replacement. We need to clean
               up the softmask stack before doing this free and creating
               a new stack. Bug 693312 */
            pdf14_free_mask_stack(ctx, ctx->memory);
        }
        ctx->mask_stack = pdf14_mask_element_new(ctx->memory);
        if (ctx->mask_stack == NULL)
            return gs_note_error(gs_error_VMerror);
        ctx->mask_stack->rc_mask = pdf14_rcmask_new(ctx->memory);
        if (ctx->mask_stack->rc_mask == NULL)
            return gs_note_error(gs_error_VMerror);
        ctx->mask_stack->rc_mask->mask_buf = tos;
    }
    return 0;
}