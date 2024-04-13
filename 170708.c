image_render_color_ht_cal(gx_image_enum *penum, const byte *buffer, int data_x,
                          uint w, int h, gx_device * dev)
{
    const byte *psrc = buffer + data_x;
    int code = 0;
    int spp_cm = 0;
    byte *psrc_cm = NULL, *psrc_cm_start = NULL;
    byte *bufend = NULL;
    byte *input[GX_DEVICE_COLOR_MAX_COMPONENTS];    /* to ensure 128 bit boundary */

    if (h == 0 || penum->line_size == 0)      /* line_size == 0, nothing to do */
        return 0;

    /* Get the buffer into the device color space */
    code = image_color_icc_prep(penum, psrc, w, dev, &spp_cm, &psrc_cm,
                                &psrc_cm_start,  &bufend, true);
    if (code < 0)
        return code;

    code = cal_halftone_process_planar(penum->cal_ht, penum->memory->non_gc_memory,
                                       input, halftone_callback, dev);

    /* Free cm buffer, if it was used */
    if (psrc_cm_start != NULL) {
        gs_free_object(penum->pgs->memory, (byte *)psrc_cm_start,
                       "image_render_color_thresh");
    }
    return code;
}