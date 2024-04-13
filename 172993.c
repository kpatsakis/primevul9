pdf14_put_image(gx_device * dev, gs_gstate * pgs, gx_device * target)
{
    const pdf14_device * pdev = (pdf14_device *)dev;
    int code;
    gs_image1_t image;
    gx_image_enum_common_t *info;
    pdf14_buf *buf = pdev->ctx->stack;
    gs_int_rect rect = buf->rect;
    int y;
    int num_comp = buf->n_chan - 1;
    byte *linebuf;
    gs_color_space *pcs;
    const byte bg = pdev->ctx->additive ? 255 : 0;
    int x1, y1, width, height;
    byte *buf_ptr;
    bool data_blended = false;
    int num_rows_left;
    gsicc_rendering_param_t render_cond;
    cmm_dev_profile_t *dev_profile;
    cmm_dev_profile_t *target_profile;

    /* Make sure that this is the only item on the stack. Fuzzing revealed a
       potential problem. Bug 694190 */
    if (buf->saved != NULL) {
        return gs_throw(gs_error_unknownerror, "PDF14 device push/pop out of sync");
    }

    if_debug0m('v', dev->memory, "[v]pdf14_put_image\n");
    rect_intersect(rect, buf->dirty);
    x1 = min(pdev->width, rect.q.x);
    y1 = min(pdev->height, rect.q.y);
    width = x1 - rect.p.x;
    height = y1 - rect.p.y;
#ifdef DUMP_TO_PNG
    dump_planar_rgba(pdev->memory, buf);
#endif
    if (width <= 0 || height <= 0 || buf->data == NULL)
        return 0;
    buf_ptr = buf->data + rect.p.y * buf->rowstride + rect.p.x;

    /* Check that target is OK.  From fuzzing results the target could have been
       destroyed, for e.g if it were a pattern accumulator that was closed
       prematurely (Bug 694154).  We should always
       be able to to get an ICC profile from the target. */
    code = dev_proc(target, get_profile)(target,  &target_profile);
    if (code < 0)
        return code;
    if (target_profile == NULL)
        return gs_throw_code(gs_error_Fatal);

    /* See if the target device has a put_image command.  If
       yes then see if it can handle the image data directly.
       If it cannot, then we will need to use the begin_typed_image
       interface, which cannot pass along tag nor alpha data to
       the target device.  Also, if a blend color space was used, we will
       also use the begin_typed_image interface */
    if (target->procs.put_image != NULL && !pdev->using_blend_cs) {
        /* See if the target device can handle the data in its current
           form with the alpha component */
        int alpha_offset = num_comp;
        int tag_offset = buf->has_tags ? num_comp+1 : 0;
        const byte *buf_ptrs[GS_CLIENT_COLOR_MAX_COMPONENTS];
        int i;
        for (i = 0; i < num_comp; i++)
            buf_ptrs[i] = buf_ptr + i * buf->planestride;
        code = dev_proc(target, put_image) (target, buf_ptrs, num_comp,
                                            rect.p.x, rect.p.y, width, height,
                                            buf->rowstride,
                                            num_comp, tag_offset);
        if (code == 0) {
            /* Device could not handle the alpha data.  Go ahead and
               preblend now. Note that if we do this, and we end up in the
               default below, we only need to repack in chunky not blend */
#if RAW_DUMP
            /* Dump before and after the blend to make sure we are doing that ok */
            dump_raw_buffer(height, width, buf->n_planes,
                        pdev->ctx->stack->planestride, pdev->ctx->stack->rowstride,
                        "pre_final_blend",buf_ptr);
            global_index++;
#endif
            gx_blend_image_buffer(buf_ptr, width, height, buf->rowstride,
                                  buf->planestride, num_comp, bg);
#if RAW_DUMP
            /* Dump before and after the blend to make sure we are doing that ok */
            dump_raw_buffer(height, width, buf->n_planes,
                        pdev->ctx->stack->planestride, pdev->ctx->stack->rowstride,
                        "post_final_blend",buf_ptr);
            global_index++;
            clist_band_count++;
#endif
            data_blended = true;
            /* Try again now */
            alpha_offset = 0;
            code = dev_proc(target, put_image) (target, buf_ptrs, num_comp,
                                                rect.p.x, rect.p.y, width, height,
                                                buf->rowstride,
                                                alpha_offset, tag_offset);
        }
        if (code > 0) {
            /* We processed some or all of the rows.  Continue until we are done */
            num_rows_left = height - code;
            while (num_rows_left > 0) {
                code = dev_proc(target, put_image) (target, buf_ptrs, buf->n_planes,
                                                    rect.p.x, rect.p.y+code, width,
                                                    num_rows_left, buf->rowstride,
                                                    alpha_offset, tag_offset);
                num_rows_left = num_rows_left - code;
            }
            return 0;
        }
    }
    /*
     * Set color space in preparation for sending an image.
     */
    code = gs_cspace_build_ICC(&pcs, NULL, pgs->memory);
    if (pcs == NULL)
        return_error(gs_error_VMerror);
    if (code < 0)
        return code;
    /* Need to set this to avoid color management during the
       image color render operation.  Exception is for the special case
       when the destination was CIELAB.  Then we need to convert from
       default RGB to CIELAB in the put image operation.  That will happen
       here as we should have set the profile for the pdf14 device to RGB
       and the target will be CIELAB.  In addition, the case when we have a
       blend color space that is different than the target device color space */
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
    image.ImageMatrix.xx = (float)width;
    image.ImageMatrix.yy = (float)height;
    image.Width = width;
    image.Height = height;
    image.BitsPerComponent = 8;
    ctm_only_writable(pgs).xx = (float)width;
    ctm_only_writable(pgs).xy = 0;
    ctm_only_writable(pgs).yx = 0;
    ctm_only_writable(pgs).yy = (float)height;
    ctm_only_writable(pgs).tx = (float)rect.p.x;
    ctm_only_writable(pgs).ty = (float)rect.p.y;
    code = dev_proc(target, begin_typed_image) (target,
                                                pgs, NULL,
                                                (gs_image_common_t *)&image,
                                                NULL, NULL, NULL,
                                                pgs->memory, &info);
    if (code < 0) {
        rc_decrement_only_cs(pcs, "pdf14_put_image");
        return code;
    }
#if RAW_DUMP
    /* Dump the current buffer to see what we have. */
    dump_raw_buffer(pdev->ctx->stack->rect.q.y-pdev->ctx->stack->rect.p.y,
                pdev->ctx->stack->rect.q.x-pdev->ctx->stack->rect.p.x,
                                pdev->ctx->stack->n_planes,
                pdev->ctx->stack->planestride, pdev->ctx->stack->rowstride,
                "pdF14_putimage",pdev->ctx->stack->data);
    dump_raw_buffer(height, width, num_comp+1,
                pdev->ctx->stack->planestride, pdev->ctx->stack->rowstride,
                "PDF14_PUTIMAGE_SMALL",buf_ptr);
    global_index++;
    if (!data_blended) {
        clist_band_count++;
    }
#endif
    linebuf = gs_alloc_bytes(pdev->memory, width * num_comp, "pdf14_put_image");
    for (y = 0; y < height; y++) {
        gx_image_plane_t planes;
        int rows_used,k,x;

        if (data_blended) {
            for (x = 0; x < width; x++) {
                for (k = 0; k < num_comp; k++) {
                    linebuf[x * num_comp + k] = buf_ptr[x + buf->planestride * k];
                }
            }
        } else {
            gx_build_blended_image_row(buf_ptr, y, buf->planestride, width,
                                       num_comp, bg, linebuf);
        }
        planes.data = linebuf;
        planes.data_x = 0;
        planes.raster = width * num_comp;
        info->procs->plane_data(info, &planes, 1, &rows_used);
        /* todo: check return value */
        buf_ptr += buf->rowstride;
    }
    gs_free_object(pdev->memory, linebuf, "pdf14_put_image");
    info->procs->end_image(info, true);
    /* This will also decrement the device profile */
    rc_decrement_only_cs(pcs, "pdf14_put_image");
    return code;
}