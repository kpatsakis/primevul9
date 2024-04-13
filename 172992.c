gx_update_pdf14_compositor(gx_device * pdev, gs_gstate * pgs,
    const gs_pdf14trans_t * pdf14pct, gs_memory_t * mem )
{
    pdf14_device *p14dev = (pdf14_device *)pdev;
    gs_pdf14trans_params_t params = pdf14pct->params;
    int code = 0;

    params.idle = pdf14pct->idle;
    switch (params.pdf14_op) {
        default:			/* Should not occur. */
            break;
        case PDF14_PUSH_DEVICE:
            if (!(params.is_pattern)) {
                p14dev->blend_mode = 0;
                p14dev->opacity = p14dev->shape = 0.0;
                pdf14_recreate_device(mem, pgs, pdev, pdf14pct);
            }
            break;
        case PDF14_ABORT_DEVICE:
            /* Something has gone very wrong.  Let transparency device clean up
               what ever it has allocated and then we are shutting it down */
            code = gx_abort_trans_device(pgs, pdev);
            if (p14dev->free_devicen) {
                devn_free_params(pdev);
            }
            pdf14_disable_device(pdev);
            pdf14_close(pdev);
            break;
        case PDF14_POP_DEVICE:
            if (!(params.is_pattern)) {
                if_debug0m('v', pdev->memory,
                           "[v]gx_update_pdf14_compositor(PDF14_POP_DEVICE)\n");
                pgs->get_cmap_procs = p14dev->save_get_cmap_procs;
                gx_set_cmap_procs(pgs, p14dev->target);
                /* Send image out raster data to output device */
                {
                    /* Make a copy so we can change the ROP */
                    gs_gstate new_pgs = *pgs;

                    /* We don't use the gs_gstate log_op since this is for the */
                    /* clist playback. Putting the image (band in the case of the */
                    /* clist) only needs to use the default ROP to copy the data  */
                    new_pgs.log_op = rop3_default;
                    code = p14dev->pdf14_procs->put_image(pdev, &new_pgs, p14dev->target);
                }
                /* Before we disable the device release any deviceN structures.
                    free_devicen is set if the pdf14 device had inherited its
                    deviceN parameters from the target clist device.  In this
                    case they should not be freed */
                if (p14dev->free_devicen) {
                    devn_free_params(pdev);
                }
                pdf14_disable_device(pdev);
                pdf14_close(pdev);
            }
            break;
        case PDF14_BEGIN_TRANS_GROUP:
            code = gx_begin_transparency_group(pgs, pdev, &params);
            break;
        case PDF14_END_TRANS_GROUP:
            code = gx_end_transparency_group(pgs, pdev);
            break;
        case PDF14_BEGIN_TRANS_MASK:
            code = gx_begin_transparency_mask(pgs, pdev, &params);
            break;
        case PDF14_END_TRANS_MASK:
            code = gx_end_transparency_mask(pgs, pdev, &params);
            break;
        case PDF14_SET_BLEND_PARAMS:
            pdf14_set_params(pgs, pdev, &pdf14pct->params);
            break;
        case PDF14_PUSH_TRANS_STATE:
            code = gx_push_transparency_state(pgs, pdev);
            break;
        case PDF14_POP_TRANS_STATE:
            code = gx_pop_transparency_state(pgs, pdev);
            break;
        case PDF14_PUSH_SMASK_COLOR:
            code = pdf14_increment_smask_color(pgs, pdev);
            break;
        case PDF14_POP_SMASK_COLOR:
            code = pdf14_decrement_smask_color(pgs, pdev);
            break;
    }
    return code;
}