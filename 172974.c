pdf14_begin_transparency_group(gx_device *dev,
                              const gs_transparency_group_params_t *ptgp,
                              const gs_rect *pbbox,
                              gs_gstate *pgs, gs_memory_t *mem)
{
    pdf14_device *pdev = (pdf14_device *)dev;
    double alpha = pgs->opacity.alpha * pgs->shape.alpha;
    gs_int_rect rect;
    int code;
    bool isolated = ptgp->Isolated;
    gs_transparency_color_t group_color;
    cmm_profile_t *group_profile;
    cmm_profile_t *tos_profile;
    gsicc_rendering_param_t render_cond;
    cmm_dev_profile_t *dev_profile;
    bool cm_back_drop = false;
    bool new_icc = false;

    code = dev_proc(dev, get_profile)(dev,  &dev_profile);
    if (code < 0)
        return code;
    gsicc_extract_profile(GS_UNKNOWN_TAG, dev_profile, &tos_profile, &render_cond);

    code = compute_group_device_int_rect(pdev, &rect, pbbox, pgs);
    if (code < 0)
        return code;
    if_debug4m('v', pdev->memory,
               "[v]pdf14_begin_transparency_group, I = %d, K = %d, alpha = %g, bm = %d\n",
               ptgp->Isolated, ptgp->Knockout, alpha, pgs->blend_mode);

    /* If the group color is unknown then use the current device profile. */
    if (ptgp->group_color == UNKNOWN){
        group_color = ICC;
        group_profile = tos_profile;
    } else {
        group_color = ptgp->group_color;
        group_profile = ptgp->iccprofile;
    }

    /* We have to handle case where the profile is in the clist */
    if (group_profile == NULL && pdev->pclist_device != NULL) {
        /* Get the serialized data from the clist. */
        gx_device_clist_reader *pcrdev = (gx_device_clist_reader *)(pdev->pclist_device);
        group_profile = gsicc_read_serial_icc((gx_device *) pcrdev, ptgp->icc_hashcode);
        if (group_profile == NULL)
            return gs_throw(gs_error_unknownerror, "ICC data not found in clist");
        /* Keep a pointer to the clist device */
        group_profile->dev = (gx_device *) pcrdev;
        new_icc = true;
    }
    if (group_profile != NULL) {
        /* If we have a non-isolated group and the color space is different,
            we will need to CM the backdrop. */
        if (!(group_profile->hash_is_valid)) {
            gsicc_get_icc_buff_hash(group_profile->buffer,
                                    &(group_profile->hashcode),
                                    group_profile->buffer_size);
            group_profile->hash_is_valid = true;
        }
        if (group_profile->hashcode != tos_profile->hashcode) {
            cm_back_drop = true;
        }
    }

    code = pdf14_update_device_color_procs(dev, group_color, ptgp->icc_hashcode,
        pgs, group_profile, false);
    if_debug0m('v', dev->memory, "[v]Transparency group color space update\n");
    if (code < 0)
        return code;
    code = pdf14_push_transparency_group(pdev->ctx, &rect, isolated, ptgp->Knockout,
                                         (byte)floor (255 * alpha + 0.5),
                                         (byte)floor (255 * pgs->shape.alpha + 0.5),
                                         pgs->blend_mode, ptgp->idle,
                                         ptgp->mask_id, pdev->color_info.num_components,
                                         cm_back_drop, group_profile, tos_profile,
                                         pgs, dev);
    if (new_icc)
        rc_decrement(group_profile, "pdf14_begin_transparency_group");
    return code;
}