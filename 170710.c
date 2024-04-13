color_halftone_init(gx_image_enum *penum)
{
    void *callback;
    void *args;
    int ox;
    int dd_curr_y;
    int dev_width;
    cal_halftone *cal_ht = NULL;
    gx_dda_fixed dda_ht;
    cal_context *ctx = penum->memory->gs_lib_ctx->core->cal_ctx;
    int k;
    gx_ht_order *d_order;
    int code;
    byte *cache = (penum->color_cache != NULL ? penum->color_cache->device_contone : NULL);
    cal_matrix matrix;
    int clip_x, clip_y;

    if (!gx_device_must_halftone(penum->dev))
        return NULL;

    if (penum->pgs == NULL || penum->pgs->dev_ht == NULL)
        return NULL;
    dda_ht = penum->dda.pixel0.x;
    if (penum->dxx > 0)
        dda_translate(dda_ht, -fixed_epsilon);
    ox = dda_current(dda_ht);
    dd_curr_y = dda_current(penum->dda.pixel0.y);
    dev_width = gxht_dda_length(&dda_ht, penum->rect.w);
    matrix.xx = penum->matrix.xx;
    matrix.xy = penum->matrix.xy;
    matrix.yx = penum->matrix.yx;
    matrix.yy = penum->matrix.yy;
    matrix.tx = penum->matrix.tx + matrix.xx * penum->rect.x + matrix.yx * penum->rect.y;
    matrix.ty = penum->matrix.ty + matrix.xy * penum->rect.x + matrix.yy * penum->rect.y;
    clip_x = fixed2int(penum->clip_outer.p.x);
    clip_y = fixed2int(penum->clip_outer.p.y);
    cal_ht = cal_halftone_init(ctx,
                               penum->memory->non_gc_memory,
                               penum->rect.w,
                               penum->rect.h,
                               &matrix,
                               penum->dev->color_info.num_components,
                               cache,
                               clip_x,
                               clip_y,
                               fixed2int_ceiling(penum->clip_outer.q.x) - clip_x,
                               fixed2int_ceiling(penum->clip_outer.q.y) - clip_y,
                               penum->adjust);
    if (cal_ht == NULL)
        goto fail;

    for (k = 0; k < penum->pgs->dev_ht->num_comp; k++) {
        d_order = &(penum->pgs->dev_ht->components[k].corder);
        code = gx_ht_construct_threshold(d_order, penum->dev, penum->pgs, k);
        if (code < 0)
            goto fail;
        if (cal_halftone_add_screen(ctx,
                                    penum->memory->non_gc_memory,
                                    cal_ht,
                                    penum->pgs->dev_ht->components[k].corder.threshold_inverted,
                                    penum->pgs->dev_ht->components[k].corder.width,
                                    penum->pgs->dev_ht->components[k].corder.full_height,
                                    -penum->pgs->screen_phase[k].x,
                                    -penum->pgs->screen_phase[k].y,
                                    penum->pgs->dev_ht->components[k].corder.threshold) < 0)
            goto fail;
    }

    return cal_ht;

fail:
    cal_halftone_fin(cal_ht, penum->memory->non_gc_memory);
    return NULL;
}