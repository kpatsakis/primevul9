pdf14_mark_fill_rectangle_ko_simple(gx_device *	dev, int x, int y, int w, int h,
                                    gx_color_index color,
                                    const gx_device_color *pdc, bool devn)
{
    pdf14_device *pdev = (pdf14_device *)dev;
    pdf14_buf *buf = pdev->ctx->stack;
    gs_blend_mode_t blend_mode = pdev->blend_mode;
    int i, j, k;
    byte *bline, *bg_ptr, *line, *dst_ptr;
    byte src[PDF14_MAX_PLANES];
    byte dst[PDF14_MAX_PLANES] = { 0 };
    int rowstride = buf->rowstride;
    int planestride = buf->planestride;
    int num_chan = buf->n_chan;
    int num_comp = num_chan - 1;
    int shape_off = num_chan * planestride;
    bool has_shape = buf->has_shape;
    bool has_alpha_g = buf->has_alpha_g;
    int alpha_g_off = shape_off + (has_shape ? planestride : 0);
    int tag_off = shape_off + (has_alpha_g ? planestride : 0) +
                              (has_shape ? planestride : 0);
    bool has_tags = buf->has_tags;
    bool additive = pdev->ctx->additive;
    gs_graphics_type_tag_t curr_tag = dev->graphics_type_tag & ~GS_DEVICE_ENCODES_TAGS;
    gx_color_index mask = ((gx_color_index)1 << 8) - 1;
    int shift = 8;
    byte shape = 0; /* Quiet compiler. */
    byte src_alpha;

    if (buf->data == NULL)
        return 0;
#if 0
    if (sizeof(color) <= sizeof(ulong))
        if_debug6m('v', dev->memory,
                   "[v]pdf14_mark_fill_rectangle_ko_simple, (%d, %d), %d x %d color = %lx, nc %d,\n",
                   x, y, w, h, (ulong)color, num_chan);
    else
        if_debug7m('v', dev->memory,
                   "[v]pdf14_mark_fill_rectangle_ko_simple, (%d, %d), %d x %d color = %8lx%08lx, nc %d,\n",
                   x, y, w, h,
                   (ulong)(color >> 8*(sizeof(color) - sizeof(ulong))), (ulong)color,
                   num_chan);
#endif
    /*
     * Unpack the gx_color_index values.  Complement the components for subtractive
     * color spaces.
     */
    if (devn) {
        if (additive) {
            for (j = 0; j < num_comp; j++) {
                src[j] = ((pdc->colors.devn.values[j]) >> shift & mask);
            }
        } else {
            for (j = 0; j < num_comp; j++) {
                src[j] = 255 - ((pdc->colors.devn.values[j]) >> shift & mask);
            }
        }
    } else
        pdev->pdf14_procs->unpack_color(num_comp, color, pdev, src);

    src_alpha = src[num_comp] = (byte)floor (255 * pdev->alpha + 0.5);
    if (has_shape) {
        shape = (byte)floor (255 * pdev->shape + 0.5);
    } else {
        shape_off = 0;
    }
    if (has_tags) {
        curr_tag = (color >> (num_comp*8)) & 0xff;
    } else {
        tag_off = 0;
    }
    if (!has_alpha_g)
        alpha_g_off = 0;
    src_alpha = 255 - src_alpha;
    shape = 255 - shape;

    /* Fit the mark into the bounds of the buffer */
    if (x < buf->rect.p.x) {
        w += x - buf->rect.p.x;
        x = buf->rect.p.x;
    }
    if (y < buf->rect.p.y) {
      h += y - buf->rect.p.y;
      y = buf->rect.p.y;
    }
    if (x + w > buf->rect.q.x) w = buf->rect.q.x - x;
    if (y + h > buf->rect.q.y) h = buf->rect.q.y - y;
    /* Update the dirty rectangle with the mark. */
    if (x < buf->dirty.p.x) buf->dirty.p.x = x;
    if (y < buf->dirty.p.y) buf->dirty.p.y = y;
    if (x + w > buf->dirty.q.x) buf->dirty.q.x = x + w;
    if (y + h > buf->dirty.q.y) buf->dirty.q.y = y + h;

    /* composite with backdrop only */
    bline = buf->backdrop + (x - buf->rect.p.x) + (y - buf->rect.p.y) * rowstride;
    line = buf->data + (x - buf->rect.p.x) + (y - buf->rect.p.y) * rowstride;

    for (j = 0; j < h; ++j) {
        bg_ptr = bline;
        dst_ptr = line;
        for (i = 0; i < w; ++i) {
            /* Complement the components for subtractive color spaces */
            if (additive) {
                for (k = 0; k < num_chan; ++k)
                    dst[k] = bg_ptr[k * planestride];
            } else {
                for (k = 0; k < num_comp; ++k)
                    dst[k] = 255 - bg_ptr[k * planestride];
            }
            dst[num_comp] = bg_ptr[num_comp * planestride];	/* alpha doesn't invert */
            if (buf->isolated) {
                art_pdf_knockoutisolated_group_8(dst, src, num_comp);
            } else {
                art_pdf_composite_knockout_8(dst, src, num_comp,
                                             blend_mode, pdev->blend_procs, pdev);
            }
            /* Complement the results for subtractive color spaces */
            if (additive) {
                for (k = 0; k < num_chan; ++k)
                    dst_ptr[k * planestride] = dst[k];
            } else {
                for (k = 0; k < num_comp; ++k)
                    dst_ptr[k * planestride] = 255 - dst[k];
                dst_ptr[num_comp * planestride] = dst[num_comp];
            }
            if (tag_off) {
                dst_ptr[tag_off] = curr_tag;
            }
            if (alpha_g_off) {
                int tmp = (255 - dst_ptr[alpha_g_off]) * src_alpha + 0x80;
                dst_ptr[alpha_g_off] = 255 - ((tmp + (tmp >> 8)) >> 8);
            }
            if (shape_off) {
                int tmp = (255 - dst_ptr[shape_off]) * shape + 0x80;
                dst_ptr[shape_off] = 255 - ((tmp + (tmp >> 8)) >> 8);
            }
            ++dst_ptr;
            ++bg_ptr;
        }
        bline += rowstride;
        line += rowstride;
    }
#if 0
/* #if RAW_DUMP */
    /* Dump the current buffer to see what we have. */
    dump_raw_buffer(pdev->ctx->stack->rect.q.y-pdev->ctx->stack->rect.p.y,
                            pdev->ctx->stack->rect.q.x-pdev->ctx->stack->rect.p.x,
                            pdev->ctx->stack->n_planes,
                            pdev->ctx->stack->planestride, pdev->ctx->stack->rowstride,
                            "Draw_Rect_KO",pdev->ctx->stack->data);
    global_index++;
#endif
    return 0;
}