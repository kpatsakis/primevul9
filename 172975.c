gs_pdf14_device_push(gs_memory_t *mem, gs_gstate * pgs,
        gx_device ** pdev, gx_device * target, const gs_pdf14trans_t * pdf14pct)
{
    pdf14_device * dev_proto;
    pdf14_device * p14dev, temp_dev_proto;
    int code;
    bool has_tags;
    cmm_profile_t *icc_profile;
    gsicc_rendering_param_t render_cond;
    cmm_dev_profile_t *dev_profile;
    uchar k;
    int max_bitmap;
    bool use_pdf14_accum = false;

    /* Guard against later seg faults, this should not be possible */
    if (target == NULL)
        return gs_throw_code(gs_error_Fatal);

    has_tags = target->graphics_type_tag & GS_DEVICE_ENCODES_TAGS;
    max_bitmap = target->space_params.MaxBitmap == 0 ? MAX_BITMAP :
                                 target->space_params.MaxBitmap;
    /* If the device is not a printer class device, it won't support saved-pages */
    /* and so we may need to make a clist device in order to prevent very large  */
    /* or high resolution pages from having allocation problems.                 */
    /* We use MaxBitmap to decide when a clist is needed.*/
    if (dev_proc(target, dev_spec_op)(target, gxdso_supports_saved_pages, NULL, 0) == 0 &&
        gx_device_is_pattern_clist(target) == 0 &&
        gx_device_is_pattern_accum(target) == 0 &&
        gs_device_is_memory(target) == 0) {

        uint32_t pdf14_trans_buffer_size = (ESTIMATED_PDF14_ROW_SPACE(max(1, target->width),
                                             target->color_info.num_components) >> 3);

        if (target->height < max_ulong / pdf14_trans_buffer_size)
                pdf14_trans_buffer_size *= target->height;
        else
                max_bitmap = 0;     /* Force decision to clist */
        if (pdf14_trans_buffer_size > max_bitmap)
            use_pdf14_accum = true;
    }
    code = dev_proc(target, get_profile)(target,  &dev_profile);
    if (code < 0)
        return code;
    gsicc_extract_profile(GS_UNKNOWN_TAG, dev_profile, &icc_profile,
                          &render_cond);
    if_debug0m('v', mem, "[v]gs_pdf14_device_push\n");

    code = get_pdf14_device_proto(target, &dev_proto, &temp_dev_proto, pgs,
                                  pdf14pct, use_pdf14_accum);
    if (code < 0)
        return code;
    code = gs_copydevice((gx_device **) &p14dev,
                         (const gx_device *) dev_proto, mem);
    if (code < 0)
        return code;
    gs_pdf14_device_copy_params((gx_device *)p14dev, target);
    gx_device_set_target((gx_device_forward *)p14dev, target);
    p14dev->pad = target->pad;
    p14dev->log2_align_mod = target->log2_align_mod;
    p14dev->is_planar = target->is_planar;
    /* If the target profile was CIELAB (and we are not using a blend CS),
       then overide with default RGB for
       proper blending.  During put_image we will convert from RGB to
       CIELAB.  Need to check that we have a default profile, which
       will not be the case if we are coming from the clist reader */
    if ((icc_profile->data_cs == gsCIELAB || icc_profile->islab)
        && pgs->icc_manager->default_rgb != NULL && !p14dev->using_blend_cs) {
        p14dev->icc_struct->device_profile[0] =
                                        pgs->icc_manager->default_rgb;
        rc_increment(pgs->icc_manager->default_rgb);
    }
    /* The number of color planes should not exceed that of the target.
       Unless we are using a blend CS */
    if (!p14dev->using_blend_cs) {
        if (p14dev->color_info.num_components > target->color_info.num_components)
            p14dev->color_info.num_components = target->color_info.num_components;
        if (p14dev->color_info.max_components > target->color_info.max_components)
            p14dev->color_info.max_components = target->color_info.max_components;
    }
    p14dev->color_info.depth = p14dev->color_info.num_components * 8;
    /* If we have a tag device then go ahead and do a special encoder
       decoder for the pdf14 device to make sure we maintain this
       information in the encoded color information.  We could use
       the target device's methods but the PDF14 device has to maintain
       8 bit color always and we could run into other issues if the number
       of colorants became large.  If we need to do compressed color with
       tags that will be a special project at that time */
    if (has_tags) {
        p14dev->procs.encode_color = pdf14_encode_color_tag;
        p14dev->color_info.depth += 8;
    }
    check_device_separable((gx_device *)p14dev);
    gx_device_fill_in_procs((gx_device *)p14dev);
    p14dev->save_get_cmap_procs = pgs->get_cmap_procs;
    pgs->get_cmap_procs = pdf14_get_cmap_procs;
    gx_set_cmap_procs(pgs, (gx_device *)p14dev);
    /* Components shift, etc have to be based upon 8 bit */
    for (k = 0; k < p14dev->color_info.num_components; k++) {
        p14dev->color_info.comp_bits[k] = 8;
        p14dev->color_info.comp_shift[k] =
                            (p14dev->color_info.num_components - 1 - k) * 8;
    }
    if (use_pdf14_accum) {
        /* we will disable this device later, but we don't want to allocate large buffers */
        p14dev->width = 1;
        p14dev->height = 1;
    }
    code = dev_proc((gx_device *) p14dev, open_device) ((gx_device *) p14dev);
    *pdev = (gx_device *) p14dev;
    pdf14_set_marking_params((gx_device *)p14dev, pgs);
    p14dev->trans_group_parent_cmap_procs = NULL;
    /* In case we have alphabits set */
    p14dev->color_info.anti_alias = target->color_info.anti_alias;
#if RAW_DUMP
    /* Dump the current buffer to see what we have. */
    dump_raw_buffer(p14dev->ctx->stack->rect.q.y-p14dev->ctx->stack->rect.p.y,
                p14dev->ctx->stack->rect.q.x-p14dev->ctx->stack->rect.p.x,
                                p14dev->ctx->stack->n_planes,
                p14dev->ctx->stack->planestride, p14dev->ctx->stack->rowstride,
                "Device_Push",p14dev->ctx->stack->data);

    global_index++;
#endif
    /* We should never go into this when using a blend color space */
    if (use_pdf14_accum) {
        const gx_device_pdf14_accum *accum_proto = NULL;
        gx_device *new_target = NULL;
        gx_device_color pdcolor;
        frac pconc_white = frac_1;

        if_debug0m('v', mem, "[v]gs_pdf14_device_push: Inserting clist device.\n");

        /* get the prototype for the accumulator device based on colorspace */
        switch (target->color_info.num_components) {
            case 1:
                accum_proto = &pdf14_accum_Gray;
                break;
            case 3:
                accum_proto = &pdf14_accum_RGB;
                break;
            case 4:
                accum_proto = &pdf14_accum_CMYK;
                break;
            default:
                /* FIXME: DeviceN ?? */
                break;		/* accum_proto will be NULL, so no accum device */
        }
        if (accum_proto == NULL ||
            (code = gs_copydevice(&new_target, (gx_device *)accum_proto, mem->stable_memory)) < 0)
            goto no_clist_accum;

        ((gx_device_pdf14_accum *)new_target)->save_p14dev = (gx_device *)p14dev;  /* non-clist p14dev */
        /* Fill in values from the target device before opening */
        new_target->color_info.separable_and_linear = GX_CINFO_SEP_LIN;
        new_target->color_info.anti_alias = p14dev->color_info.anti_alias;
        set_linear_color_bits_mask_shift(new_target);
        gs_pdf14_device_copy_params(new_target, target);
        ((gx_device_pdf14_accum *)new_target)->page_uses_transparency = true;
        gx_device_fill_in_procs(new_target);

        memcpy(&(new_target->space_params), &(target->space_params), sizeof(gdev_space_params));
        max_bitmap = max(target->space_params.MaxBitmap, target->space_params.BufferSpace);
        new_target->space_params.BufferSpace = max_bitmap;

        new_target->PageHandlerPushed = true;
        new_target->ObjectHandlerPushed = true;

        if ((code = gdev_prn_open(new_target)) < 0 ||
             !PRINTER_IS_CLIST((gx_device_printer *)new_target)) {
            gs_free_object(mem->stable_memory, new_target, "pdf14-accum");
            goto no_clist_accum;
        }
        /* Do the initial fillpage into the pdf14-accum device we just created */
        dev_proc(new_target, set_graphics_type_tag)(new_target, GS_PATH_TAG);
        if ((code = gx_remap_concrete_DGray(&pconc_white,
                                            gs_currentcolorspace_inline((gs_gstate *)pgs),
                                            &pdcolor, pgs, new_target, gs_color_select_all)) < 0)
            goto no_clist_accum;

        (*dev_proc(new_target, fillpage))(new_target, pgs, &pdcolor);
        code = clist_create_compositor(new_target, pdev, (gs_composite_t *)pdf14pct, pgs, mem, NULL);
        if (code < 0)
            goto no_clist_accum;

        pdf14_disable_device((gx_device *)p14dev);           /* make the non-clist device forward */
        pdf14_close((gx_device *)p14dev);                    /* and free up the little memory it had */
    }
    return code;

no_clist_accum:
        /* FIXME: We allocated a really small p14dev, but that won't work */
    return gs_throw_code(gs_error_Fatal); /* punt for now */
}