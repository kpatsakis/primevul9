image_render_color_icc_tpr(gx_image_enum *penum_orig, const byte *buffer, int data_x,
                          uint w, int h, gx_device * dev)
{
    const gx_image_enum *const penum = penum_orig; /* const within proc */
    const gs_gstate *pgs = penum->pgs;
    int spp = penum->spp;
    const byte *psrc = buffer + data_x * spp;
    int code;
    byte *psrc_cm = NULL, *psrc_cm_start = NULL;
    byte *psrc_cm_initial;
    byte *bufend = NULL;
    int spp_cm = 0;
    bool must_halftone = penum->icc_setup.must_halftone;
    bool has_transfer = penum->icc_setup.has_transfer;
    gx_cmapper_t cmapper;
    transform_pixel_region_data data;

    if (h == 0)
        return 0;
    code = image_color_icc_prep(penum_orig, psrc, w, dev, &spp_cm, &psrc_cm,
                                &psrc_cm_start, &bufend, false);
    if (code < 0) return code;
    psrc_cm_initial = psrc_cm;
    gx_get_cmapper(&cmapper, pgs, dev, has_transfer, must_halftone, gs_color_select_source);

    data.state = penum->tpr_state;
    data.u.process_data.buffer[0] = psrc_cm;
    data.u.process_data.data_x = 0;
    data.u.process_data.cmapper = &cmapper;
    code = dev_proc(dev, transform_pixel_region)(dev, transform_pixel_region_process_data, &data);
    gs_free_object(pgs->memory, (byte *)psrc_cm_start, "image_render_color_icc");

    if (code < 0) {
        /* Save position if error, in case we resume. */
        penum_orig->used.x = (data.u.process_data.buffer[0] - psrc_cm_initial) / spp_cm;
        penum_orig->used.y = 0;
    }
    return code;
}