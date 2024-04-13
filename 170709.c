gs_image_class_4_color(gx_image_enum * penum, irender_proc_t *render_fn)
{
    bool std_cmap_procs;
    int code = 0;
#if USE_FAST_HT_CODE
    bool use_fast_thresh = true;
#else
    bool use_fast_thresh = false;
#endif
    const gs_color_space *pcs;
    gsicc_rendering_param_t rendering_params;
    int k;
    int src_num_comp = cs_num_components(penum->pcs);
    int des_num_comp, bpc;
    cmm_dev_profile_t *dev_profile;

    if (penum->use_mask_color) {
        /*
         * Scale the mask colors to match the scaling of each sample to
         * a full byte, and set up the quick-filter parameters.
         */
        int i;
        color_samples mask, test;
        bool exact = penum->spp <= BYTES_PER_BITS32;

        memset(&mask, 0, sizeof(mask));
        memset(&test, 0, sizeof(test));
        for (i = 0; i < penum->spp; ++i) {
            byte v0, v1;
            byte match = 0xff;

            gx_image_scale_mask_colors(penum, i);
            v0 = (byte)penum->mask_color.values[2 * i];
            v1 = (byte)penum->mask_color.values[2 * i + 1];
            while ((v0 & match) != (v1 & match))
                match <<= 1;
            mask.v[i] = match;
            test.v[i] = v0 & match;
            exact &= (v0 == match && (v1 | match) == 0xff);
        }
        penum->mask_color.mask = mask.all[0];
        penum->mask_color.test = test.all[0];
        penum->mask_color.exact = exact;
    } else {
        penum->mask_color.mask = 0;
        penum->mask_color.test = ~0;
    }
    /* If the device has some unique color mapping procs due to its color space,
       then we will need to use those and go through pixel by pixel instead
       of blasting through buffers.  This is true for example with many of
       the color spaces for CUPs */
    std_cmap_procs = gx_device_uses_std_cmap_procs(penum->dev, penum->pgs);
    if ( (gs_color_space_get_index(penum->pcs) == gs_color_space_index_DeviceN &&
        penum->pcs->cmm_icc_profile_data == NULL) || penum->use_mask_color ||
        !std_cmap_procs) {
         *render_fn = &image_render_color_DeviceN;
         return code;
    }

    /* Set up the link now */
    code = dev_proc(penum->dev, get_profile)(penum->dev, &dev_profile);
    if (code < 0)
        return code;

    des_num_comp = gsicc_get_device_profile_comps(dev_profile);
    bpc = penum->dev->color_info.depth / des_num_comp;	/* bits per component */
    penum->icc_setup.need_decode = false;
    /* Check if we need to do any decoding.  If yes, then that will slow us down */
    for (k = 0; k < src_num_comp; k++) {
        if ( penum->map[k].decoding != sd_none ) {
            penum->icc_setup.need_decode = true;
            break;
        }
    }
    /* Define the rendering intents */
    rendering_params.black_point_comp = penum->pgs->blackptcomp;
    rendering_params.graphics_type_tag = GS_IMAGE_TAG;
    rendering_params.override_icc = false;
    rendering_params.preserve_black = gsBKPRESNOTSPECIFIED;
    rendering_params.rendering_intent = penum->pgs->renderingintent;
    rendering_params.cmm = gsCMM_DEFAULT;
    if (gs_color_space_is_PSCIE(penum->pcs) && penum->pcs->icc_equivalent != NULL) {
        pcs = penum->pcs->icc_equivalent;
    } else {
        pcs = penum->pcs;
    }
    penum->icc_setup.is_lab = pcs->cmm_icc_profile_data->islab;
    penum->icc_setup.must_halftone = gx_device_must_halftone(penum->dev);
    penum->icc_setup.has_transfer = gx_has_transfer(penum->pgs, des_num_comp);
    if (penum->icc_setup.is_lab)
        penum->icc_setup.need_decode = false;
    if (penum->icc_link == NULL) {
        penum->icc_link = gsicc_get_link(penum->pgs, penum->dev, pcs, NULL,
            &rendering_params, penum->memory);
    }
    /* PS CIE color spaces may have addition decoding that needs to
       be performed to ensure that the range of 0 to 1 is provided
       to the CMM since ICC profiles are restricted to that range
       but the PS color spaces are not. */
    penum->use_cie_range = false;
    if (gs_color_space_is_PSCIE(penum->pcs) &&
        penum->pcs->icc_equivalent != NULL) {
        /* We have a PS CIE space.  Check the range */
        if ( !check_cie_range(penum->pcs) ) {
            /* It is not 0 to 1.  We will be doing decode
               plus an additional linear adjustment */
            penum->use_cie_range = (get_cie_range(penum->pcs) != NULL);
        }
    }
    if (gx_device_must_halftone(penum->dev) && use_fast_thresh &&
        (penum->posture == image_portrait || penum->posture == image_landscape)
        && penum->image_parent_type == gs_image_type1) {
        bool transfer_is_monotonic = true;

        for (k=0; k<des_num_comp; k++) {
            if (!gx_transfer_is_monotonic(penum->pgs, k)) {
                transfer_is_monotonic = false;
                break;
            }
        }
        /* If num components is 1 or if we are going to CMYK planar device
           then we will may use the thresholding if it is a halftone
           device IFF we have one bit per component */
        if ((bpc == 1) && transfer_is_monotonic &&
            (penum->dev->color_info.num_components == 1 || penum->dev->is_planar) &&
            penum->bps == 8) {
#ifdef WITH_CAL
            penum->cal_ht = color_halftone_init(penum);
            if (penum->cal_ht != NULL)
            {
                penum->skip_render = image_render_color_ht_cal;
                return code;
            }
#else
            code = gxht_thresh_image_init(penum);
            if (code == 0) {
                 /* NB: transfer function is pickled into the threshold arrray */
                 penum->icc_setup.has_transfer = false;
                 *render_fn = &image_render_color_thresh;
                 return code;
            }
#endif
        }
    }
    {
        gs_int_rect rect;
        transform_pixel_region_data data;
        data.u.init.clip = &rect;
        data.u.init.w = penum->rect.w;
        data.u.init.h = penum->rect.h;
        data.u.init.pixels = &penum->dda.pixel0;
        data.u.init.rows = &penum->dda.row;
        data.u.init.lop = penum->log_op;
        rect.p.x = fixed2int(penum->clip_outer.p.x);
        rect.p.y = fixed2int(penum->clip_outer.p.y);
        rect.q.x = fixed2int_ceiling(penum->clip_outer.q.x);
        rect.q.y = fixed2int_ceiling(penum->clip_outer.q.y);

        if (penum->icc_link == NULL || (penum->icc_link->is_identity && !penum->icc_setup.need_decode))
            data.u.init.spp = penum->spp;
        else
            data.u.init.spp = des_num_comp;

        code = dev_proc(penum->dev, transform_pixel_region)(penum->dev, transform_pixel_region_begin, &data);
        if (code >= 0) {
            penum->tpr_state = data.state;
            penum->skip_next_line = image_skip_color_icc_tpr;
            *render_fn = &image_render_color_icc_tpr;
            return code;
        }
    }
    return code;
}