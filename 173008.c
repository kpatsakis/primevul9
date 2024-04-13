pdf14_end_transparency_group(gx_device *dev,
                              gs_gstate *pgs)
{
    pdf14_device *pdev = (pdf14_device *)dev;
    int code;
    pdf14_parent_color_t *parent_color;
    cmm_profile_t *group_profile;
    gsicc_rendering_param_t render_cond;
    cmm_dev_profile_t *dev_profile;

    code = dev_proc(dev, get_profile)(dev,  &dev_profile);
    if (code < 0)
        return code;
    gsicc_extract_profile(GS_UNKNOWN_TAG, dev_profile, &group_profile,
                          &render_cond);
    if_debug0m('v', dev->memory, "[v]pdf14_end_transparency_group\n");
    code = pdf14_pop_transparency_group(pgs, pdev->ctx, pdev->blend_procs,
                                pdev->color_info.num_components, group_profile,
                                (gx_device *) pdev);
#ifdef DEBUG
    pdf14_debug_mask_stack_state(pdev->ctx);
#endif
   /* May need to reset some color stuff related
     * to a mismatch between the parents color space
     * and the group blending space */
    parent_color = pdev->ctx->stack->parent_color_info_procs;
        if (!(parent_color->parent_color_mapping_procs == NULL &&
                parent_color->parent_color_comp_index == NULL)) {
            pgs->get_cmap_procs = parent_color->get_cmap_procs;
            gx_set_cmap_procs(pgs, dev);
            pdev->procs.get_color_mapping_procs =
                parent_color->parent_color_mapping_procs;
            pdev->procs.get_color_comp_index =
                parent_color->parent_color_comp_index;
            pdev->color_info.polarity = parent_color->polarity;
            pdev->color_info.num_components = parent_color->num_components;
            pdev->blend_procs = parent_color->parent_blending_procs;
            pdev->ctx->additive = parent_color->isadditive;
            pdev->pdf14_procs = parent_color->unpack_procs;
            pdev->color_info.depth = parent_color->depth;
            pdev->color_info.max_color = parent_color->max_color;
            pdev->color_info.max_gray = parent_color->max_gray;
            memcpy(&(pdev->color_info.comp_bits),&(parent_color->comp_bits),
                            GX_DEVICE_COLOR_MAX_COMPONENTS);
            memcpy(&(pdev->color_info.comp_shift),&(parent_color->comp_shift),
                            GX_DEVICE_COLOR_MAX_COMPONENTS);
            parent_color->get_cmap_procs = NULL;
            parent_color->parent_color_comp_index = NULL;
            parent_color->parent_color_mapping_procs = NULL;
        if (parent_color->icc_profile != NULL) {
            /* make sure to decrement the device profile.  If it was allocated
               with the push then it will be freed. */
            rc_decrement(group_profile,"pdf14_end_transparency_group");
            dev->icc_struct->device_profile[0] = parent_color->icc_profile;
            rc_decrement(parent_color->icc_profile,"pdf14_end_transparency_group");
            parent_color->icc_profile = NULL;
        }
    }
    return code;
}