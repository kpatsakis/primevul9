pdf14_update_device_color_procs_pop_c(gx_device *dev,gs_gstate *pgs)
{

    pdf14_device *pdev = (pdf14_device *)dev;
    pdf14_parent_color_t *parent_color = pdev->trans_group_parent_cmap_procs;
    gx_device_clist_writer * cldev = (gx_device_clist_writer *)pdev->pclist_device;

    if_debug0m('v', pdev->memory, "[v]pdf14_update_device_color_procs_pop_c\n");
    /* The color procs are always pushed.  Simply restore them. */
    if (!(parent_color->parent_color_mapping_procs == NULL &&
        parent_color->parent_color_comp_index == NULL)) {
        if_debug2m('v', pdev->memory,
                   "[v]pdf14_update_device_color_procs_pop_c,num_components_old = %d num_components_new = %d\n",
                   pdev->color_info.num_components,parent_color->num_components);
        pgs->get_cmap_procs = parent_color->get_cmap_procs;
        gx_set_cmap_procs(pgs, dev);
        pdev->procs.get_color_mapping_procs = parent_color->parent_color_mapping_procs;
        pdev->procs.get_color_comp_index = parent_color->parent_color_comp_index;
        pdev->color_info.polarity = parent_color->polarity;
        pdev->color_info.depth = parent_color->depth;
        pdev->color_info.num_components = parent_color->num_components;
        pdev->blend_procs = parent_color->parent_blending_procs;
        pdev->pdf14_procs = parent_color->unpack_procs;
        pdev->color_info.max_color = parent_color->max_color;
        pdev->color_info.max_gray = parent_color->max_gray;
        pdev->procs.encode_color = parent_color->encode;
        pdev->procs.decode_color = parent_color->decode;
        memcpy(&(pdev->color_info.comp_bits),&(parent_color->comp_bits),
                            GX_DEVICE_COLOR_MAX_COMPONENTS);
        memcpy(&(pdev->color_info.comp_shift),&(parent_color->comp_shift),
                            GX_DEVICE_COLOR_MAX_COMPONENTS);
        /* clist writer fill rect has no access to gs_gstate */
        /* and it forwards the target device.  this information */
        /* is passed along to use in this case */
        cldev->clist_color_info.depth = pdev->color_info.depth;
        cldev->clist_color_info.polarity = pdev->color_info.polarity;
        cldev->clist_color_info.num_components = pdev->color_info.num_components;
        cldev->clist_color_info.max_color = pdev->color_info.max_color;
        cldev->clist_color_info.max_gray = pdev->color_info.max_gray;
        memcpy(&(cldev->clist_color_info.comp_bits),&(parent_color->comp_bits),
                            GX_DEVICE_COLOR_MAX_COMPONENTS);
        memcpy(&(cldev->clist_color_info.comp_shift),&(parent_color->comp_shift),
                            GX_DEVICE_COLOR_MAX_COMPONENTS);
        if (pdev->ctx){
            pdev->ctx->additive = parent_color->isadditive;
        }
       /* The device profile must be restored. */
         dev->icc_struct->device_profile[0] = parent_color->icc_profile;
         rc_decrement(parent_color->icc_profile, "pdf14_update_device_color_procs_pop_c");
         parent_color->icc_profile = NULL;
         if_debug0m('v', dev->memory, "[v]procs updated\n");
    } else {
        if_debug0m('v', dev->memory, "[v]pdf14_update_device_color_procs_pop_c ERROR \n");
    }
    pdf14_pop_parent_color(dev, pgs);
    return 0;
}