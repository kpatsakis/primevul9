image_render_color_DeviceN(gx_image_enum *penum_orig, const byte *buffer, int data_x,
                   uint w, int h, gx_device * dev)
{
    const gx_image_enum *const penum = penum_orig; /* const within proc */
    const gs_gstate *pgs = penum->pgs;
    gs_logical_operation_t lop = penum->log_op;
    gx_dda_fixed_point pnext;
    image_posture posture = penum->posture;
    fixed xprev, yprev;
    fixed pdyx, pdyy;		/* edge of parallelogram */
    int vci, vdi;
    const gs_color_space *pcs = penum->pcs;
    cs_proc_remap_color((*remap_color)) = pcs->type->remap_color;
    gs_client_color cc;
    gx_device_color devc1;
    gx_device_color devc2;
    gx_device_color *pdevc;
    gx_device_color *pdevc_next;
    gx_device_color *ptemp;
    int spp = penum->spp;
    const byte *psrc_initial = buffer + data_x * spp;
    const byte *psrc = psrc_initial;
    const byte *rsrc = psrc + spp; /* psrc + spp at start of run */
    fixed xrun;			/* x ditto */
    fixed yrun;			/* y ditto */
    int irun;			/* int x/rrun */
    color_samples run;		/* run value */
    color_samples next;		/* next sample value */
    const byte *bufend = psrc + w;
    int code = 0, mcode = 0;
    int i;
    bits32 mask = penum->mask_color.mask;
    bits32 test = penum->mask_color.test;
    bool lab_case = false;

    if (h == 0)
        return 0;

    /* Decide on which remap proc to use.  If the source colors are LAB
       then use the mapping that does not rescale the source colors */
    if (gs_color_space_is_ICC(pcs) && pcs->cmm_icc_profile_data != NULL &&
        pcs->cmm_icc_profile_data->islab) {
        remap_color = gx_remap_ICC_imagelab;
        lab_case = true;
    } else {
        remap_color = pcs->type->remap_color;
    }
    pdevc = &devc1;
    pdevc_next = &devc2;
    /* In case these are devn colors */
    if (dev_proc(dev, dev_spec_op)(dev, gxdso_supports_devn, NULL, 0)) {
        for (i = 0; i < GS_CLIENT_COLOR_MAX_COMPONENTS; i++) {
            pdevc->colors.devn.values[i] = 0;
            pdevc_next->colors.devn.values[i] = 0;
        }
    }
    /* These used to be set by init clues */
    pdevc->type = gx_dc_type_none;
    pdevc_next->type = gx_dc_type_none;
    pnext = penum->dda.pixel0;
    xrun = xprev = dda_current(pnext.x);
    yrun = yprev = dda_current(pnext.y);
    pdyx = dda_current(penum->dda.row.x) - penum->cur.x;
    pdyy = dda_current(penum->dda.row.y) - penum->cur.y;
    switch (posture) {
        case image_portrait:
            vci = penum->yci, vdi = penum->hci;
            irun = fixed2int_var_rounded(xrun);
            break;
        case image_landscape:
        default:    /* we don't handle skew -- treat as landscape */
            vci = penum->xci, vdi = penum->wci;
            irun = fixed2int_var_rounded(yrun);
            break;
    }
    if_debug5m('b', penum->memory, "[b]y=%d data_x=%d w=%d xt=%f yt=%f\n",
               penum->y, data_x, w, fixed2float(xprev), fixed2float(yprev));
    memset(&run, 0, sizeof(run));
    memset(&next, 0, sizeof(next));
    cs_full_init_color(&cc, pcs);
    run.v[0] = ~psrc[0];	/* force remap */
    while (psrc < bufend) {
        dda_next(pnext.x);
        dda_next(pnext.y);
        if (posture != image_skewed && !memcmp(psrc, run.v, spp)) {
            psrc += spp;
            goto inc;
        }
        memcpy(next.v, psrc, spp);
        psrc += spp;
    /* Check for transparent color. */
        if ((next.all[0] & mask) == test &&
            (penum->mask_color.exact ||
             mask_color_matches(next.v, penum, spp))) {
            color_set_null(pdevc_next);
            goto mapped;
        }
        /* Data is already properly set up for ICC use of LAB */
        if (lab_case)
            for (i = 0; i < spp; ++i)
                cc.paint.values[i] = (next.v[i]) * (1.0f / 255.0f);
        else
            for (i = 0; i < spp; ++i)
                decode_sample(next.v[i], cc, i);
#ifdef DEBUG
        if (gs_debug_c('B')) {
            dmprintf2(dev->memory, "[B]cc[0..%d]=%g", spp - 1,
                     cc.paint.values[0]);
            for (i = 1; i < spp; ++i)
                dmprintf1(dev->memory, ",%g", cc.paint.values[i]);
            dmputs(dev->memory, "\n");
        }
#endif
        mcode = remap_color(&cc, pcs, pdevc_next, pgs, dev,
                           gs_color_select_source);
mapped:	if (mcode < 0)
            goto fill;
        if (sizeof(pdevc_next->colors.binary.color[0]) <= sizeof(ulong))
            if_debug7m('B', penum->memory,
                       "[B]0x%x,0x%x,0x%x,0x%x -> 0x%lx,0x%lx,0x%lx\n",
                       next.v[0], next.v[1], next.v[2], next.v[3],
                       (ulong)pdevc_next->colors.binary.color[0],
                       (ulong)pdevc_next->colors.binary.color[1],
                       (ulong) pdevc_next->type);
        else
            if_debug9m('B', penum->memory,
                       "[B]0x%x,0x%x,0x%x,0x%x -> 0x%08lx%08lx,0x%08lx%08lx,0x%lx\n",
                       next.v[0], next.v[1], next.v[2], next.v[3],
                       (ulong)(pdevc_next->colors.binary.color[0] >>
                               8 * (sizeof(pdevc_next->colors.binary.color[0]) - sizeof(ulong))),
                       (ulong)pdevc_next->colors.binary.color[0],
                       (ulong)(pdevc_next->colors.binary.color[1] >>
                               8 * (sizeof(pdevc_next->colors.binary.color[1]) - sizeof(ulong))),
                       (ulong)pdevc_next->colors.binary.color[1],
                       (ulong) pdevc_next->type);
        /* NB: printf above fails to account for sizeof gx_color_index 4 or 8 bytes */
        if (posture != image_skewed && dev_color_eq(*pdevc, *pdevc_next))
            goto set;
fill:	/* Fill the region between */
        /* xrun/irun and xprev */
        /*
         * Note;  This section is nearly a copy of a simlar section below
         * for processing the last image pixel in the loop.  This would have been
         * made into a subroutine except for complications about the number of
         * variables that would have been needed to be passed to the routine.
         */
        switch (posture) {
        case image_portrait:
            {		/* Rectangle */
                int xi = irun;
                int wi = (irun = fixed2int_var_rounded(xprev)) - xi;

                if (wi < 0)
                    xi += wi, wi = -wi;
                if (wi > 0)
                    code = gx_fill_rectangle_device_rop(xi, vci, wi, vdi,
                                                        pdevc, dev, lop);
            }
            break;
        case image_landscape:
            {		/* 90 degree rotated rectangle */
                int yi = irun;
                int hi = (irun = fixed2int_var_rounded(yprev)) - yi;

                if (hi < 0)
                    yi += hi, hi = -hi;
                if (hi > 0)
                    code = gx_fill_rectangle_device_rop(vci, yi, vdi, hi,
                                                        pdevc, dev, lop);
            }
            break;
        default:
            {		/* Parallelogram */
                code = (*dev_proc(dev, fill_parallelogram))
                    (dev, xrun, yrun, xprev - xrun, yprev - yrun, pdyx, pdyy,
                     pdevc, lop);
                xrun = xprev;
                yrun = yprev;
            }
        }
        if (code < 0)
            goto err;
        rsrc = psrc;

        if ((code = mcode) < 0) goto err;
        /* Swap around the colors due to a change */
        ptemp = pdevc;
        pdevc = pdevc_next;
        pdevc_next = ptemp;
set:	run = next;
inc:	xprev = dda_current(pnext.x);
        yprev = dda_current(pnext.y);	/* harmless if no skew */
    }
    /* Fill the last run. */
    /*
     * Note;  This section is nearly a copy of a simlar section above
     * for processing an image pixel in the loop.  This would have been
     * made into a subroutine except for complications about the number
     * variables that would have been needed to be passed to the routine.
     */
    switch (posture) {
        case image_portrait:
            {		/* Rectangle */
                int xi = irun;
                int wi = (irun = fixed2int_var_rounded(xprev)) - xi;

                if (wi < 0)
                    xi += wi, wi = -wi;
                if (wi > 0)
                    code = gx_fill_rectangle_device_rop(xi, vci, wi, vdi,
                                                        pdevc, dev, lop);
            }
            break;
        case image_landscape:
            {		/* 90 degree rotated rectangle */
                int yi = irun;
                int hi = (irun = fixed2int_var_rounded(yprev)) - yi;

                if (hi < 0)
                    yi += hi, hi = -hi;
                if (hi > 0)
                    code = gx_fill_rectangle_device_rop(vci, yi, vdi, hi,
                                                        pdevc, dev, lop);
            }
            break;
        default:
            {		/* Parallelogram */
                code = (*dev_proc(dev, fill_parallelogram))
                    (dev, xrun, yrun, xprev - xrun, yprev - yrun, pdyx, pdyy,
                     pdevc, lop);
            }
    }
    return (code < 0 ? code : 1);
    /* Save position if error, in case we resume. */
err:
    penum_orig->used.x = (rsrc - spp - psrc_initial) / spp;
    penum_orig->used.y = 0;
    return code;
}