pdf14_update_device_color_procs_push_c(gx_device *dev,
                              gs_transparency_color_t group_color, int64_t icc_hashcode,
                              gs_gstate *pgs, cmm_profile_t *icc_profile, bool is_mask)
{
    pdf14_device *pdevproto;
    pdf14_device *pdev = (pdf14_device *)dev;
    gx_device_clist_writer * cldev = (gx_device_clist_writer *)pdev->pclist_device;
    const pdf14_procs_t *new_14procs;
    bool update_color_info;
    gx_color_polarity_t new_polarity;
    int new_num_comps;
    bool new_additive = false;
    byte new_depth;
    byte comp_bits[GX_DEVICE_COLOR_MAX_COMPONENTS];
    byte comp_shift[GX_DEVICE_COLOR_MAX_COMPONENTS];
    int k;
    bool has_tags = dev->graphics_type_tag & GS_DEVICE_ENCODES_TAGS;
    cmm_profile_t *icc_profile_dev;
    gsicc_rendering_param_t render_cond;
    cmm_dev_profile_t *dev_profile;

    memset(comp_bits, 0, GX_DEVICE_COLOR_MAX_COMPONENTS);
    memset(comp_shift, 0, GX_DEVICE_COLOR_MAX_COMPONENTS);

    if (group_color == ICC && icc_profile == NULL)
        return gs_throw(gs_error_undefinedresult, "Missing ICC data");
    if_debug0m('v', cldev->memory, "[v]pdf14_update_device_color_procs_push_c\n");
   /* Check if we need to alter the device procs at this stage.  Many of the procs
      are based upon the color space of the device.  We want to remain in the
      color space defined by the color space of the soft mask or transparency
      group as opposed to the device color space. Later, when we pop the softmask
      we will collapse it to a single band and then compose with it to the device
      color space (or the parent layer space).  In the case where we pop an
      isolated transparency group, we will do the blending in the proper color
      space and then transform the data when we pop the group.  Remember that only
      isolated groups can have color spaces that are different than their parent. */
        update_color_info = false;
        switch (group_color) {
            case GRAY_SCALE:
                  if (pdev->color_info.num_components != 1){
                    update_color_info = true;
                    new_polarity = GX_CINFO_POLARITY_ADDITIVE;
                    new_num_comps = 1;
                    pdevproto = (pdf14_device *)&gs_pdf14_Gray_device;
                    new_additive = true;
                    new_14procs = &gray_pdf14_procs;
                    new_depth = 8;
                    comp_bits[0] = 8;
                    comp_shift[0] = 0;
                }
                break;
            case DEVICE_RGB:
            case CIE_XYZ:
                if (pdev->color_info.num_components != 3){
                    update_color_info = true;
                    new_polarity = GX_CINFO_POLARITY_ADDITIVE;
                    new_num_comps = 3;
                    pdevproto = (pdf14_device *)&gs_pdf14_RGB_device;
                    new_additive = true;
                    new_14procs = &rgb_pdf14_procs;
                    new_depth = 24;
                    for (k = 0; k < 3; k++) {
                        comp_bits[k] = 8;
                        comp_shift[k] = (2-k)*8;
                    }
                }
                break;
            case DEVICE_CMYK:
                if (pdev->color_info.num_components != 4){
                    update_color_info = true;
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
                    new_depth = 32;
                    for (k = 0; k < 4; k++) {
                        comp_bits[k] = 8;
                        comp_shift[k] = (3-k)*8;
                    }
                }
                break;
            case ICC:
                /* Check if the profile is different. */
                dev_proc(dev, get_profile)(dev,  &dev_profile);
                gsicc_extract_profile(GS_UNKNOWN_TAG, dev_profile,
                                      &icc_profile_dev, &render_cond);
                if (icc_profile_dev->hashcode != icc_profile->hashcode) {
                    update_color_info = true;
                    new_num_comps = icc_profile->num_comps;
                    new_depth = icc_profile->num_comps * 8;
                    switch (new_num_comps) {
                    case 1:
                        if (pdev->sep_device && !is_mask) {
                            pdevproto = (pdf14_device *)&gs_pdf14_Grayspot_device;
                            new_14procs = &grayspot_pdf14_procs;
                        } else {
                            pdevproto = (pdf14_device *)&gs_pdf14_Gray_device;
                            new_14procs = &gray_pdf14_procs;
                        }
                        new_polarity = GX_CINFO_POLARITY_ADDITIVE;
                        new_additive = true;
                        comp_bits[0] = 8;
                        comp_shift[0] = 0;
                        break;
                    case 3:
                        if (pdev->sep_device) {
                            pdevproto = (pdf14_device *)&gs_pdf14_RGBspot_device;
                            new_14procs = &rgbspot_pdf14_procs;
                        } else {
                            pdevproto = (pdf14_device *)&gs_pdf14_RGB_device;
                            new_14procs = &rgb_pdf14_procs;
                        }
                        new_polarity = GX_CINFO_POLARITY_ADDITIVE;
                        new_additive = true;
                        for (k = 0; k < 3; k++) {
                            comp_bits[k] = 8;
                            comp_shift[k] = (2-k)*8;
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
                        new_polarity = GX_CINFO_POLARITY_SUBTRACTIVE;
                        new_additive = false;
                        for (k = 0; k < 4; k++) {
                            comp_bits[k] = 8;
                            comp_shift[k] = (3-k)*8;
                        }
                        break;
                    default:
                        return gs_throw(gs_error_undefinedresult,
                                        "ICC Number of colorants illegal");
                    }
                }
                break;
            case UNKNOWN:
                return 0;
                break;
            default:
                return_error(gs_error_rangecheck);
                break;
         }
         if (update_color_info){
             if (pdev->sep_device && !is_mask) {
                 int num_spots = pdev->color_info.num_components -
                     icc_profile_dev->num_comps;
                 if (num_spots > 0) {
                     new_num_comps += num_spots;
                     for (k = 0; k < new_num_comps; k++) {
                         comp_bits[k] = 8;
                         comp_shift[k] = (new_num_comps - k - 1) * 8;

                     }
                 }
             }
             if (has_tags) {
                 new_depth += 8;
             }
            if_debug2m('v', pdev->memory,
                       "[v]pdf14_update_device_color_procs_push_c,num_components_old = %d num_components_new = %d\n",
                       pdev->color_info.num_components,new_num_comps);
            /* Set new information in the device */
            pdev->procs.get_color_mapping_procs =
                pdevproto->static_procs->get_color_mapping_procs;
            pdev->procs.get_color_comp_index =
                pdevproto->static_procs->get_color_comp_index;
            pdev->blend_procs = pdevproto->blend_procs;
            pdev->color_info.polarity = new_polarity;
            pdev->color_info.num_components = new_num_comps;
            pdev->color_info.max_color = 255;
            pdev->color_info.max_gray = 255;
            pdev->pdf14_procs = new_14procs;
            pdev->color_info.depth = new_num_comps * 8;
            memset(&(pdev->color_info.comp_bits),0,GX_DEVICE_COLOR_MAX_COMPONENTS);
            memset(&(pdev->color_info.comp_shift),0,GX_DEVICE_COLOR_MAX_COMPONENTS);
            memcpy(&(pdev->color_info.comp_bits),comp_bits,4);
            memcpy(&(pdev->color_info.comp_shift),comp_shift,4);
            /* If we have a compressed color codec, and we are doing a soft mask
               push operation then go ahead and update the color encode and
               decode for the pdf14 device to not used compressed color
               encoding while in the soft mask.  We will just check for gray
               and compressed.  Note that we probably don't have_tags if we
               are dealing with compressed color.  But is is possible so
               we add it in to catch for future use. */
            cldev->clist_color_info.depth = pdev->color_info.depth;
            cldev->clist_color_info.polarity = pdev->color_info.polarity;
            cldev->clist_color_info.num_components = pdev->color_info.num_components;
            cldev->clist_color_info.max_color = pdev->color_info.max_color;
            cldev->clist_color_info.max_gray = pdev->color_info.max_gray;
            /* For the ICC profiles, we want to update the ICC profile for the
               device in the ICC manager.  We already stored in in pdf14_parent_color_t.
               That will be stored in the clist and restored during the reading phase. */
           if (group_color == ICC) {
                dev->icc_struct->device_profile[0] = icc_profile;
            }
            if (pdev->ctx) {
               pdev->ctx->additive = new_additive;
            }
            return(1);  /* Lets us detect that we did do an update */
         }
         if_debug0m('v', pdev->memory, "[v]procs not updated\n");
         return 0;
}