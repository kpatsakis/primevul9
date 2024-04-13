pdf14_push_parent_color(gx_device *dev, const gs_gstate *pgs)
{
    pdf14_device *pdev = (pdf14_device *)dev;
    pdf14_parent_color_t *new_parent_color;
    cmm_profile_t *icc_profile;
    gsicc_rendering_param_t render_cond;
    cmm_dev_profile_t *dev_profile;

    dev_proc(dev, get_profile)(dev,  &dev_profile);
    gsicc_extract_profile(GS_UNKNOWN_TAG, dev_profile, &icc_profile,
                          &render_cond);
    if_debug0m('v', dev->memory, "[v]pdf14_push_parent_color\n");
    /* Allocate a new one */
    new_parent_color = gs_alloc_struct(dev->memory, pdf14_parent_color_t,
                                        &st_pdf14_clr,"pdf14_clr_new");
    /* Link to old one */
    new_parent_color->previous = pdev->trans_group_parent_cmap_procs;
    /* Reassign new one to dev */
    pdev->trans_group_parent_cmap_procs = new_parent_color;
    /* Initialize with values */
    new_parent_color->get_cmap_procs = pgs->get_cmap_procs;
    new_parent_color->parent_color_mapping_procs =
        pdev->procs.get_color_mapping_procs;
    new_parent_color->parent_color_comp_index =
        pdev->procs.get_color_comp_index;
    new_parent_color->parent_blending_procs = pdev->blend_procs;
    new_parent_color->polarity = pdev->color_info.polarity;
    new_parent_color->num_components = pdev->color_info.num_components;
    new_parent_color->unpack_procs = pdev->pdf14_procs;
    new_parent_color->depth = pdev->color_info.depth;
    new_parent_color->max_color = pdev->color_info.max_color;
    new_parent_color->max_gray = pdev->color_info.max_gray;
    new_parent_color->decode = pdev->procs.decode_color;
    new_parent_color->encode = pdev->procs.encode_color;
    memcpy(&(new_parent_color->comp_bits),&(pdev->color_info.comp_bits),
                        GX_DEVICE_COLOR_MAX_COMPONENTS);
    memcpy(&(new_parent_color->comp_shift),&(pdev->color_info.comp_shift),
                        GX_DEVICE_COLOR_MAX_COMPONENTS);
    /* The ICC manager has the ICC profile for the device */
    new_parent_color->icc_profile = icc_profile;
    rc_increment(icc_profile);
    /* isadditive is only used in ctx */
    if (pdev->ctx) {
        new_parent_color->isadditive = pdev->ctx->additive;
    }
}