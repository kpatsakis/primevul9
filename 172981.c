pdf14_cmap_separation_direct(frac all, gx_device_color * pdc, const gs_gstate * pgs,
                 gx_device * dev, gs_color_select_t select)
{
    int i, ncomps = dev->color_info.num_components;
    int num_spots = pdf14_get_num_spots(dev);
    bool additive = dev->color_info.polarity == GX_CINFO_POLARITY_ADDITIVE;
    frac comp_value = all;
    frac cm_comps[GX_DEVICE_COLOR_MAX_COMPONENTS];
    gx_color_value cv[GX_DEVICE_COLOR_MAX_COMPONENTS];
    gx_color_index color;

    if (pgs->color_component_map.sep_type == SEP_ALL) {
        /*
         * Invert the photometric interpretation for additive
         * color spaces because separations are always subtractive.
         */
        if (additive)
            comp_value = frac_1 - comp_value;
        /* Use the "all" value for all components */
        i = pgs->color_component_map.num_colorants - 1;
        for (; i >= 0; i--)
            cm_comps[i] = comp_value;
    } else {
        /* map to the color model */
        map_components_to_colorants(&comp_value, &(pgs->color_component_map), cm_comps);
    }
    /* apply the transfer function(s); convert to color values */
    if (additive) {
        for (i = 0; i < ncomps; i++)
            cv[i] = frac2cv(gx_map_color_frac(pgs, cm_comps[i], effective_transfer[i]));
        /* We are in an additive mode (blend space) and drawing with a sep color
        into a sep device.  Make sure we are drawing "white" with the process
        colorants, but only if we are not in an ALL case */
        if (pgs->color_component_map.sep_type != SEP_ALL)
            for (i = 0; i < ncomps - num_spots; i++)
                cv[i] = gx_max_color_value;
    } else
        for (i = 0; i < ncomps; i++)
            cv[i] = frac2cv(frac_1 - gx_map_color_frac(pgs, (frac)(frac_1 - cm_comps[i]), effective_transfer[i]));


    /* if output device supports devn, we need to make sure we send it the
       proper color type */
    if (dev_proc(dev, dev_spec_op)(dev, gxdso_supports_devn, NULL, 0)) {
        for (i = 0; i < ncomps; i++)
            pdc->colors.devn.values[i] = cv[i];
        pdc->type = gx_dc_type_devn;
    } else {
        /* encode as a color index */
        color = dev_proc(dev, encode_color)(dev, cv);
        /* check if the encoding was successful; we presume failure is rare */
        if (color != gx_no_color_index)
            color_set_pure(pdc, color);
    }
}