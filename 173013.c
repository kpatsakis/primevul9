pdf14_clist_create_compositor(gx_device	* dev, gx_device ** pcdev,
    const gs_composite_t * pct, gs_gstate * pgs, gs_memory_t * mem,
    gx_device *cdev)
{
    pdf14_clist_device * pdev = (pdf14_clist_device *)dev;
    int code, is_pdf14_compositor;
    const gs_pdf14trans_t * pdf14pct = (const gs_pdf14trans_t *) pct;

    /* We only handle a few PDF 1.4 transparency operations */
    if ((is_pdf14_compositor = gs_is_pdf14trans_compositor(pct)) != 0) {
        switch (pdf14pct->params.pdf14_op) {
            case PDF14_PUSH_DEVICE:
                /* Re-activate the PDF 1.4 compositor */
                pdev->saved_target_color_info = pdev->target->color_info;
                pdev->target->color_info = pdev->color_info;
                pdev->saved_target_encode_color = pdev->target->procs.encode_color;
                pdev->saved_target_decode_color = pdev->target->procs.decode_color;
                pdev->target->procs.encode_color = pdev->procs.encode_color =
                                                   pdev->my_encode_color;
                pdev->target->procs.decode_color = pdev->procs.decode_color =
                                                   pdev->my_decode_color;
                pdev->saved_target_get_color_mapping_procs =
                                    pdev->target->procs.get_color_mapping_procs;
                pdev->saved_target_get_color_comp_index =
                                        pdev->target->procs.get_color_comp_index;
                pdev->target->procs.get_color_mapping_procs =
                        pdev->procs.get_color_mapping_procs =
                        pdev->my_get_color_mapping_procs;
                pdev->target->procs.get_color_comp_index =
                        pdev->procs.get_color_comp_index =
                        pdev->my_get_color_comp_index;
                pdev->save_get_cmap_procs = pgs->get_cmap_procs;
                pgs->get_cmap_procs = pdf14_get_cmap_procs;
                gx_set_cmap_procs(pgs, dev);
                code = pdf14_recreate_clist_device(mem, pgs, dev, pdf14pct);
                pdev->blend_mode = pdev->text_knockout = 0;
                pdev->opacity = pdev->shape = 0.0;
                if (code < 0)
                    return code;
                /*
                 * This routine is part of the PDF 1.4 clist write device.
                 * Change the compositor procs to not create another since we
                 * do not need to create a chain of identical devices.
                 */
                {
                    gs_composite_t pctemp = *pct;

                    pctemp.type = &gs_composite_pdf14trans_no_clist_writer_type;
                    code = dev_proc(pdev->target, create_compositor)
                                (pdev->target, pcdev, &pctemp, pgs, mem, cdev);
                    *pcdev = dev;
                    return code;
                }
            case PDF14_POP_DEVICE:
                /* Restore the color_info for the clist device */
                pdev->target->color_info = pdev->saved_target_color_info;
                pdev->target->procs.encode_color =
                                        pdev->saved_target_encode_color;
                pdev->target->procs.decode_color =
                                        pdev->saved_target_decode_color;
                pdev->target->procs.get_color_mapping_procs =
                                    pdev->saved_target_get_color_mapping_procs;
                pdev->target->procs.get_color_comp_index =
                                    pdev->saved_target_get_color_comp_index;
                pgs->get_cmap_procs = pdev->save_get_cmap_procs;
                gx_set_cmap_procs(pgs, pdev->target);
                gx_device_decache_colors(pdev->target);
                /* Disable the PDF 1.4 compositor */
                pdf14_disable_clist_device(mem, pgs, dev);
                /*
                 * Make sure that the transfer funtions, etc. are current.
                 */
                code = cmd_put_color_mapping(
                        (gx_device_clist_writer *)(pdev->target), pgs);
                if (code < 0)
                    return code;
                break;
            case PDF14_BEGIN_TRANS_GROUP:
                /*
                 * Keep track of any changes made in the blending parameters.
                   These need to be written out in the same bands as the group
                   information is written.  Hence the passing of the dimensions
                   for the group. */
                code = pdf14_clist_update_params(pdev, pgs, true,
                                                 (gs_pdf14trans_params_t *)&(pdf14pct->params));
                if (code < 0)
                    return code;
                if (pdf14pct->params.Background_components != 0 &&
                    pdf14pct->params.Background_components !=
                    pdev->color_info.num_components)
                    return_error(gs_error_rangecheck);
                /* We need to update the clist writer device procs based upon the
                   the group color space.  For simplicity, the list item is
                   created even if the color space did not change */
                /* First store the current ones */
                pdf14_push_parent_color(dev, pgs);

                code = pdf14_update_device_color_procs_push_c(dev,
                                pdf14pct->params.group_color,
                                pdf14pct->params.icc_hash, pgs,
                                pdf14pct->params.iccprofile, false);
                if (code < 0)
                    return code;
                break;
            case PDF14_BEGIN_TRANS_MASK:
                /* We need to update the clist writer device procs based upon the
                   the group color space.  For simplicity, the list item is created
                   even if the color space did not change */
                /* First store the current ones */
                if (pdf14pct->params.subtype == TRANSPARENCY_MASK_None)
                    break;
                pdf14_push_parent_color(dev, pgs);
                /* If we are playing back from a clist, the iccprofile may need to be loaded */
                if (pdf14pct->params.iccprofile == NULL) {
                    gs_pdf14trans_params_t *pparams_noconst = (gs_pdf14trans_params_t *)&(pdf14pct->params);

                    pparams_noconst->iccprofile = gsicc_read_serial_icc((gx_device *) cdev,
                                                       pdf14pct->params.icc_hash);
                    if (pparams_noconst->iccprofile == NULL)
                        return gs_throw(-1, "ICC data not found in clist");
                    /* Keep a pointer to the clist device */
                    pparams_noconst->iccprofile->dev = (gx_device *)cdev;
                    /* Now we need to load the rest of the profile buffer */
                    if (pparams_noconst->iccprofile->buffer == NULL) {
                        gcmmhprofile_t dummy = gsicc_get_profile_handle_clist(pparams_noconst->iccprofile, mem);

                        if (dummy == NULL)
                            return_error(gs_error_VMerror);
                    }
                }
                /* Now update the device procs */
                code = pdf14_update_device_color_procs_push_c(dev,
                                  pdf14pct->params.group_color,
                                  pdf14pct->params.icc_hash, pgs,
                                  pdf14pct->params.iccprofile, true);
                if (code < 0)
                    return code;
                /* Also, if the BC is a value that may end up as something other
                  than transparent. We must use the parent colors bounding box in
                  determining the range of bands in which this mask can affect.
                  So, if needed change the masks bounding box at this time */
                break;
            case PDF14_END_TRANS_GROUP:
            case PDF14_END_TRANS_MASK:
                /* We need to update the clist writer device procs based upon the
                   the group color space. */
                code = pdf14_update_device_color_procs_pop_c(dev,pgs);
                if (code < 0)
                    return code;
                break;
            case PDF14_PUSH_TRANS_STATE:
                break;
            case PDF14_POP_TRANS_STATE:
                break;
            case PDF14_PUSH_SMASK_COLOR:
                code = pdf14_increment_smask_color(pgs,dev);
                *pcdev = dev;
                return code;  /* Note, this are NOT put in the clist */
                break;
            case PDF14_POP_SMASK_COLOR:
                code = pdf14_decrement_smask_color(pgs,dev);
                *pcdev = dev;
                return code;  /* Note, this are NOT put in the clist */
                break;
            case PDF14_SET_BLEND_PARAMS:
                /* If there is a change we go ahead and apply it to the target */
                code = pdf14_clist_update_params(pdev, pgs, false,
                                                 (gs_pdf14trans_params_t *)&(pdf14pct->params));
                *pcdev = dev;
                return code;
                break;
            case PDF14_ABORT_DEVICE:
                break;
            default:
                break;		/* Pass remaining ops to target */
        }
    }
    code = dev_proc(pdev->target, create_compositor)
                        (pdev->target, pcdev, pct, pgs, mem, cdev);
    /* If we were accumulating into a pdf14-clist-accum device, */
    /* we now have to render the page into it's target device */
    if (is_pdf14_compositor && pdf14pct->params.pdf14_op == PDF14_POP_DEVICE &&
        pdev->target->stype == &st_pdf14_accum) {

        int y, rows_used;
        byte *linebuf = gs_alloc_bytes(mem, gx_device_raster((gx_device *)pdev, true), "pdf14-clist_accum pop dev");
        byte *actual_data;
        gx_device *tdev = pdev->target;     /* the printer class clist device used to accumulate */
        /* get the target device we want to send the image to */
        gx_device *target = ((pdf14_device *)((gx_device_pdf14_accum *)(tdev))->save_p14dev)->target;
        gs_image1_t image;
        gs_color_space *pcs;
        gx_image_enum_common_t *info;
        gx_image_plane_t planes;
        gsicc_rendering_param_t render_cond;
        cmm_dev_profile_t *dev_profile;

        /*
         * Set color space in preparation for sending an image.
         */
        code = gs_cspace_build_ICC(&pcs, NULL, pgs->memory);
        if (linebuf == NULL || pcs == NULL)
            goto put_accum_error;

        /* Need to set this to avoid color management during the
           image color render operation.  Exception is for the special case
           when the destination was CIELAB.  Then we need to convert from
           default RGB to CIELAB in the put image operation.  That will happen
           here as we should have set the profile for the pdf14 device to RGB
           and the target will be CIELAB */
        code = dev_proc(dev, get_profile)(dev,  &dev_profile);
        if (code < 0) {
            rc_decrement_only_cs(pcs, "pdf14_put_image");
            return code;
        }
        gsicc_extract_profile(GS_UNKNOWN_TAG, dev_profile,
                              &(pcs->cmm_icc_profile_data), &render_cond);
        /* pcs takes a reference to the profile data it just retrieved. */
        rc_increment(pcs->cmm_icc_profile_data);
        gscms_set_icc_range(&(pcs->cmm_icc_profile_data));

        gs_image_t_init_adjust(&image, pcs, false);
        image.ImageMatrix.xx = (float)pdev->width;
        image.ImageMatrix.yy = (float)pdev->height;
        image.Width = pdev->width;
        image.Height = pdev->height;
        image.BitsPerComponent = 8;
        ctm_only_writable(pgs).xx = (float)pdev->width;
        ctm_only_writable(pgs).xy = 0;
        ctm_only_writable(pgs).yx = 0;
        ctm_only_writable(pgs).yy = (float)pdev->height;
        ctm_only_writable(pgs).tx = 0.0;
        ctm_only_writable(pgs).ty = 0.0;
        code = dev_proc(target, begin_typed_image) (target,
                                                    pgs, NULL,
                                                    (gs_image_common_t *)&image,
                                                    NULL, NULL, NULL,
                                                    pgs->memory, &info);
        if (code < 0)
            goto put_accum_error;
        for (y=0; y < tdev->height; y++) {
            code = dev_proc(tdev, get_bits)(tdev, y, linebuf, &actual_data);
            planes.data = actual_data;
            planes.data_x = 0;
            planes.raster = tdev->width * tdev->color_info.num_components;
            if ((code = info->procs->plane_data(info, &planes, 1, &rows_used)) < 0)
                goto put_accum_error;
        }
        info->procs->end_image(info, true);

put_accum_error:
        gs_free_object(pdev->memory, linebuf, "pdf14_put_image");
        /* This will also decrement the device profile */
        rc_decrement_only_cs(pcs, "pdf14_put_image");
        dev_proc(tdev, close_device)(tdev);	/* frees the prn_device memory */
        /* Now unhook the clist device and hook to the original so we can clean up */
        gx_device_set_target((gx_device_forward *)pdev,
                             ((gx_device_pdf14_accum *)(pdev->target))->save_p14dev);
        pdev->pclist_device = pdev->target;         /* FIXME: is this kosher ? */
        *pcdev = pdev->target;			    /* pass upwards to switch devices */
        pdev->color_info = target->color_info;      /* same as in pdf14_disable_clist */
        gs_free_object(tdev->memory, tdev, "popdevice pdf14-accum");
        return 0;		/* DON'T perform set_target */
    }

    if (*pcdev != pdev->target)
        gx_device_set_target((gx_device_forward *)pdev, *pcdev);
    *pcdev = dev;
    return code;
}