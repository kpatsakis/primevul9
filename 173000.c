pdf14_end_transparency_mask(gx_device *dev, gs_gstate *pgs)
{
    pdf14_device *pdev = (pdf14_device *)dev;
    pdf14_parent_color_t *parent_color;
    int ok;

    if_debug0m('v', dev->memory, "pdf14_end_transparency_mask\n");
    ok = pdf14_pop_transparency_mask(pdev->ctx, pgs, dev);
#ifdef DEBUG
    pdf14_debug_mask_stack_state(pdev->ctx);
#endif
    /* May need to reset some color stuff related
     * to a mismatch between the Smask color space
     * and the Smask blending space */
    if (pdev->ctx->stack != NULL ) {
        parent_color = pdev->ctx->stack->parent_color_info_procs;
        if (!(parent_color->parent_color_mapping_procs == NULL &&
            parent_color->parent_color_comp_index == NULL)) {
            pgs->get_cmap_procs = parent_color->get_cmap_procs;
            gx_set_cmap_procs(pgs, dev);
            pdev->procs.get_color_mapping_procs = parent_color->parent_color_mapping_procs;
            pdev->procs.get_color_comp_index = parent_color->parent_color_comp_index;
            pdev->color_info.polarity = parent_color->polarity;
            pdev->color_info.num_components = parent_color->num_components;
            pdev->color_info.depth = parent_color->depth;
            pdev->blend_procs = parent_color->parent_blending_procs;
            pdev->ctx->additive = parent_color->isadditive;
            pdev->pdf14_procs = parent_color->unpack_procs;
            pdev->color_info.max_color = parent_color->max_color;
            pdev->color_info.max_gray = parent_color->max_gray;
            parent_color->get_cmap_procs = NULL;
            parent_color->parent_color_comp_index = NULL;
            parent_color->parent_color_mapping_procs = NULL;
            pdev->procs.encode_color = parent_color->encode;
            pdev->procs.decode_color = parent_color->decode;
            memcpy(&(pdev->color_info.comp_bits),&(parent_color->comp_bits),
                                GX_DEVICE_COLOR_MAX_COMPONENTS);
            memcpy(&(pdev->color_info.comp_shift),&(parent_color->comp_shift),
                                GX_DEVICE_COLOR_MAX_COMPONENTS);
            /* Take care of the ICC profile */
            if (parent_color->icc_profile != NULL) {
                rc_decrement(dev->icc_struct->device_profile[0],"pdf14_end_transparency_mask");
                dev->icc_struct->device_profile[0] = parent_color->icc_profile;
                rc_decrement(parent_color->icc_profile,"pdf14_end_transparency_mask");
                parent_color->icc_profile = NULL;
            }
        }
    }
    return ok;
}