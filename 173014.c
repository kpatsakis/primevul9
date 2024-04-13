pdf14_begin_transparency_mask(gx_device	*dev,
                              const gx_transparency_mask_params_t *ptmp,
                              const gs_rect *pbbox,
                              gs_gstate *pgs, gs_memory_t *mem)
{
    pdf14_device *pdev = (pdf14_device *)dev;
    byte bg_alpha = 0;   /* By default the background alpha (area outside mask) is zero */
    byte *transfer_fn;
    gs_int_rect rect;
    int code;
    int group_color_numcomps;
    gs_transparency_color_t group_color;

    if (ptmp->subtype == TRANSPARENCY_MASK_None) {
        pdf14_ctx *ctx = pdev->ctx;

        /* free up any maskbuf on the current tos */
        if (ctx->mask_stack) {
            if (ctx->mask_stack->rc_mask->mask_buf != NULL ) {
                pdf14_buf_free(ctx->mask_stack->rc_mask->mask_buf, ctx->mask_stack->memory);
                ctx->mask_stack->rc_mask->mask_buf = NULL;
            }
        }
        return 0;
    }
    transfer_fn = (byte *)gs_alloc_bytes(pdev->ctx->memory, 256,
                                               "pdf14_begin_transparency_mask");
    if (transfer_fn == NULL)
        return_error(gs_error_VMerror);
    code = compute_group_device_int_rect(pdev, &rect, pbbox, pgs);
    if (code < 0)
        return code;
    /* If we have background components the background alpha may be nonzero */
    if (ptmp->Background_components)
        bg_alpha = (int)(255 * ptmp->GrayBackground + 0.5);
    if_debug1m('v', dev->memory,
               "pdf14_begin_transparency_mask, bg_alpha = %d\n", bg_alpha);
    memcpy(transfer_fn, ptmp->transfer_fn, size_of(ptmp->transfer_fn));
   /* If the group color is unknown, then we must use the previous group color
       space or the device process color space */
    if (ptmp->group_color == UNKNOWN){
        if (pdev->ctx->stack){
            /* Use previous group color space */
            group_color_numcomps = pdev->ctx->stack->n_chan-1;  /* Remove alpha */
        } else {
            /* Use process color space */
            group_color_numcomps = pdev->color_info.num_components;
        }
        switch (group_color_numcomps) {
            case 1:
                group_color = GRAY_SCALE;
                break;
            case 3:
                group_color = DEVICE_RGB;
                break;
            case 4:
                group_color = DEVICE_CMYK;
            break;
            default:
                /* We can end up here if we are in a deviceN color space and
                   we have a sep output device */
                group_color = DEVICEN;
            break;
         }
    } else {
        group_color = ptmp->group_color;
        group_color_numcomps = ptmp->group_color_numcomps;
    }
    /* Always update the color mapping procs.  Otherwise we end up
       fowarding to the target device. */
    code = pdf14_update_device_color_procs(dev, group_color, ptmp->icc_hashcode,
                                           pgs, ptmp->iccprofile, true);
    if (code < 0)
        return code;
    /* Note that the soft mask always follows the group color requirements even
       when we have a separable device */
    return pdf14_push_transparency_mask(pdev->ctx, &rect, bg_alpha,
                                        transfer_fn, ptmp->idle, ptmp->replacing,
                                        ptmp->mask_id, ptmp->subtype,
                                        group_color_numcomps,
                                        ptmp->Background_components,
                                        ptmp->Background,
                                        ptmp->Matte_components,
                                        ptmp->Matte,
                                        ptmp->GrayBackground);
}