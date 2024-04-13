pdf14_update_device_color_procs(gx_device *dev,
                              gs_transparency_color_t group_color,
                              int64_t icc_hashcode, gs_gstate *pgs,
                              cmm_profile_t *iccprofile, bool is_mask)
{
    pdf14_device *pdevproto = NULL;
    pdf14_device *pdev = (pdf14_device *)dev;
    const pdf14_procs_t *new_14procs = NULL;
    pdf14_parent_color_t *parent_color_info;
    gx_color_polarity_t new_polarity;
    uchar new_num_comps;
    bool new_additive;
    gx_device_clist_reader *pcrdev;
    byte comp_bits[GX_DEVICE_COLOR_MAX_COMPONENTS];
    byte comp_shift[GX_DEVICE_COLOR_MAX_COMPONENTS];
    int k;
    bool has_tags = dev->graphics_type_tag & GS_DEVICE_ENCODES_TAGS;
    gsicc_rendering_param_t render_cond;
    cmm_dev_profile_t *dev_profile;

    if (pdev->ctx->stack != NULL){
        parent_color_info = pdev->ctx->stack->parent_color_info_procs;
    } else {
        /* This should not occur */
        return_error(gs_error_undefined);
    }
    if_debug0m('v', dev->memory, "[v]pdf14_update_device_color_procs\n");

    memset(comp_bits, 0, GX_DEVICE_COLOR_MAX_COMPONENTS);
    memset(comp_shift, 0, GX_DEVICE_COLOR_MAX_COMPONENTS);

    /* Update the device procs at this stage.  Many of the procs are based upon
       the color space of the device.  We want to remain in the color space
       defined by the color space of the soft mask or transparency group as
       opposed to the device color space. Later, when we pop the softmask we will
       collapse it to a single band and then compose with it to the device color
       space (or the parent layer space).  In the case where we pop an isolated
       transparency group, we will do the blending in the proper color space and
       then transform the data when we pop the group. Remember that only isolated
       groups can have color spaces that are different than their parent.
       Separation devices that have to maintain separate spot color planes need
       special handling here */
    parent_color_info->get_cmap_procs = NULL;
    parent_color_info->parent_color_mapping_procs = NULL;
    parent_color_info->parent_color_comp_index = NULL;

    switch (group_color) {
        case GRAY_SCALE:
            new_polarity = GX_CINFO_POLARITY_ADDITIVE;
            new_num_comps = 1;
            pdevproto = (pdf14_device *)&gs_pdf14_Gray_device;
            new_additive = true;
            new_14procs = &gray_pdf14_procs;
            comp_bits[0] = 8;
            comp_shift[0] = 0;
            break;
        case DEVICE_RGB:
        case CIE_XYZ:
            new_polarity = GX_CINFO_POLARITY_ADDITIVE;
            new_num_comps = 3;
            pdevproto = (pdf14_device *)&gs_pdf14_RGB_device;
            new_additive = true;
            new_14procs = &rgb_pdf14_procs;
            for (k = 0; k < 3; k++) {
                comp_bits[k] = 8;
                comp_shift[k] = (2 - k) * 8;
            }
            break;
        case DEVICE_CMYK:
            new_polarity = GX_CINFO_POLARITY_SUBTRACTIVE;
            new_num_comps = 4;
            pdevproto = (pdf14_device *)&gs_pdf14_CMYK_device;
            new_additive = false;
            /* This is needed due to the mismatched compressed encode decode
                between the device procs and the pdf14 procs */
            if (dev->color_info.num_components > 4){
                new_14procs = &cmykspot_pdf14_procs;
            } else {
                new_14procs = &cmyk_pdf14_procs;
            }
            for (k = 0; k < 4; k++) {
                comp_bits[k] = 8;
                comp_shift[k] = (3 - k) * 8;
            }
            break;
        case ICC:
            /* If we are coming from the clist reader, then we need to get
                the ICC data now  */
            if (iccprofile == NULL && pdev->pclist_device != NULL) {
                /* Get the serialized data from the clist.  Not the whole
                    profile. */
                pcrdev = (gx_device_clist_reader *)(pdev->pclist_device);
                iccprofile = gsicc_read_serial_icc((gx_device *) pcrdev,
                                                    icc_hashcode);
                if (iccprofile == NULL)
                    return gs_throw(gs_error_unknownerror, "ICC data not found in clist");
                /* Keep a pointer to the clist device */
                iccprofile->dev = (gx_device *) pcrdev;
            } else {
                /* Go ahead and rc increment right now.  This way when
                    we pop, we will make sure to decrement and avoid a
                    leak for the above profile that we just created */
                if (iccprofile == NULL)
                    return gs_throw(gs_error_unknownerror, "ICC data unknown");
                rc_increment(iccprofile);
            }
            new_num_comps = iccprofile->num_comps;
            if (new_num_comps == 4) {
                new_additive = false;
                new_polarity = GX_CINFO_POLARITY_SUBTRACTIVE;
            } else {
                new_additive = true;
                new_polarity = GX_CINFO_POLARITY_ADDITIVE;
            }
            switch (new_num_comps) {
                case 1:
                    if (pdev->sep_device && !is_mask) {
                        pdevproto = (pdf14_device *)&gs_pdf14_Grayspot_device;
                        new_14procs = &grayspot_pdf14_procs;
                    } else {
                        pdevproto = (pdf14_device *)&gs_pdf14_Gray_device;
                        new_14procs = &gray_pdf14_procs;
                    }
                    comp_bits[0] = 8;
                    comp_shift[0] = 0;
                    break;
                case 3:
                    if (pdev->sep_device) {
                        pdevproto = (pdf14_device *)&gs_pdf14_RGBspot_device;
                        new_14procs = &rgbspot_pdf14_procs;
                    }
                    else {
                        pdevproto = (pdf14_device *)&gs_pdf14_RGB_device;
                        new_14procs = &rgb_pdf14_procs;
                    }
                    for (k = 0; k < 3; k++) {
                        comp_bits[k] = 8;
                        comp_shift[k] = (2 - k) * 8;
                    }
                    break;
                case 4:
                    if (pdev->sep_device) {
                        pdevproto = (pdf14_device *)&gs_pdf14_CMYKspot_device;
                        new_14procs = &cmykspot_pdf14_procs;
                    } else {
                        pdevproto = (pdf14_device *)&gs_pdf14_CMYK_device;
                        new_14procs = &cmyk_pdf14_procs;
                    }
                    for (k = 0; k < 4; k++) {
                        comp_bits[k] = 8;
                        comp_shift[k] = (3 - k) * 8;
                    }
                    break;
                default:
                    return_error(gs_error_rangecheck);
                    break;
            }
            break;
        default:
            return_error(gs_error_rangecheck);
            break;
        }
    if_debug2m('v', pdev->memory,
                "[v]pdf14_update_device_color_procs,num_components_old = %d num_components_new = %d\n",
                pdev->color_info.num_components,new_num_comps);

    /* Save the old information */
    parent_color_info->get_cmap_procs = pgs->get_cmap_procs;
    parent_color_info->parent_color_mapping_procs =
        pdev->procs.get_color_mapping_procs;
    parent_color_info->parent_color_comp_index =
        pdev->procs.get_color_comp_index;
    parent_color_info->parent_blending_procs = pdev->blend_procs;
    parent_color_info->polarity =              pdev->color_info.polarity;
    parent_color_info->num_components =        pdev->color_info.num_components;
    parent_color_info->isadditive =            pdev->ctx->additive;
    parent_color_info->unpack_procs =          pdev->pdf14_procs;
    parent_color_info->depth =                 pdev->color_info.depth;
    parent_color_info->max_color =             pdev->color_info.max_color;
    parent_color_info->max_gray =              pdev->color_info.max_gray;
    parent_color_info->encode =                pdev->procs.encode_color;
    parent_color_info->decode =                pdev->procs.decode_color;
    memcpy(&(parent_color_info->comp_bits), &(pdev->color_info.comp_bits),
        GX_DEVICE_COLOR_MAX_COMPONENTS);
    memcpy(&(parent_color_info->comp_shift), &(pdev->color_info.comp_shift),
        GX_DEVICE_COLOR_MAX_COMPONENTS);

    /* Don't increment the space since we are going to remove it from the
        ICC manager anyway.  */
    if (group_color == ICC && iccprofile != NULL) {
        dev_proc(dev, get_profile)(dev, &dev_profile);
        gsicc_extract_profile(GS_UNKNOWN_TAG, dev_profile,
                                &(parent_color_info->icc_profile), &render_cond);
    }
    /* Set new information */
    /* If we are a sep device and this is not a softmask, ensure we maintain the
       spot colorants and know how to index into them */
    if (pdev->sep_device && !is_mask) {
        int num_spots = parent_color_info->num_components -
            parent_color_info->icc_profile->num_comps;

        if (num_spots > 0) {
            new_num_comps += num_spots;
            for (k = 0; k < new_num_comps; k++) {
                comp_bits[k] = 8;
                comp_shift[k] = (new_num_comps - k - 1) * 8;
            }
        }
    }

    pdev->procs.get_color_mapping_procs =
        pdevproto->static_procs->get_color_mapping_procs;
    pdev->procs.get_color_comp_index =
        pdevproto->static_procs->get_color_comp_index;
    pdev->blend_procs = pdevproto->blend_procs;
    pdev->color_info.polarity = new_polarity;
    pdev->color_info.num_components = new_num_comps;
    pdev->ctx->additive = new_additive;
    pdev->pdf14_procs = new_14procs;
    pdev->color_info.depth = new_num_comps * 8;
    if (has_tags) {
        pdev->color_info.depth += 8;
    }
    memset(&(pdev->color_info.comp_bits), 0, GX_DEVICE_COLOR_MAX_COMPONENTS);
    memset(&(pdev->color_info.comp_shift), 0, GX_DEVICE_COLOR_MAX_COMPONENTS);
    memcpy(&(pdev->color_info.comp_bits), comp_bits, 4);
    memcpy(&(pdev->color_info.comp_shift), comp_shift, 4);
    pdev->color_info.max_color = 255;
    pdev->color_info.max_gray = 255;
    /* If the CS was ICC based, we need to update the device ICC profile
        in the ICC manager, since that is the profile that is used for the
        PDF14 device */
    if (group_color == ICC && iccprofile != NULL) {
        /* iccprofile was incremented above if we had not just created it.
           When we do the pop we will decrement and if we just created it, it
           will be destroyed */
        dev->icc_struct->device_profile[0] = iccprofile;
        rc_increment(parent_color_info->icc_profile);
    }
    return 1;  /* Lets us detect that we did do an update */
}