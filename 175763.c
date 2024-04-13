check_device_separable(gx_device * dev)
{
    int i, j;
    gx_device_color_info * pinfo = &(dev->color_info);
    int num_components = pinfo->num_components;
    byte comp_shift[GX_DEVICE_COLOR_MAX_COMPONENTS];
    byte comp_bits[GX_DEVICE_COLOR_MAX_COMPONENTS];
    gx_color_index comp_mask[GX_DEVICE_COLOR_MAX_COMPONENTS];
    gx_color_index color_index;
    gx_color_index current_bits = 0;
    gx_color_value colorants[GX_DEVICE_COLOR_MAX_COMPONENTS] = { 0 };

    /* If this is already known then we do not need to do anything. */
    if (pinfo->separable_and_linear != GX_CINFO_UNKNOWN_SEP_LIN)
        return;
    /* If there is not an encode_color_routine then we cannot proceed. */
    if (dev_proc(dev, encode_color) == NULL)
        return;
    /*
     * If these values do not check then we should have an error.  However
     * we do not know what to do so we are simply exitting and hoping that
     * the device will clean up its values.
     */
    if (pinfo->gray_index < num_components &&
        (!pinfo->dither_grays || pinfo->dither_grays != (pinfo->max_gray + 1)))
            return;
    if ((num_components > 1 || pinfo->gray_index != 0) &&
        (!pinfo->dither_colors || pinfo->dither_colors != (pinfo->max_color + 1)))
        return;
    /*
     * If dither_grays or dither_colors is not a power of two then we assume
     * that the device is not separable.  In theory this not a requirement
     * but it has been true for all of the devices that we have seen so far.
     * This assumption also makes the logic in the next section easier.
     */
    if (!is_power_of_two(pinfo->dither_grays)
                    || !is_power_of_two(pinfo->dither_colors))
        return;
    /*
     * Use the encode_color routine to try to verify that the device is
     * separable and to determine the shift count, etc. for each colorant.
     */
    color_index = dev_proc(dev, encode_color)(dev, colorants);
    if (color_index != 0)
        return;		/* Exit if zero colorants produce a non zero index */
    for (i = 0; i < num_components; i++) {
        /* Check this colorant = max with all others = 0 */
        for (j = 0; j < num_components; j++)
            colorants[j] = 0;
        colorants[i] = gx_max_color_value;
        color_index = dev_proc(dev, encode_color)(dev, colorants);
        if (color_index == 0)	/* If no bits then we have a problem */
            return;
        if (color_index & current_bits)	/* Check for overlapping bits */
            return;
        current_bits |= color_index;
        comp_mask[i] = color_index;
        /* Determine the shift count for the colorant */
        for (j = 0; (color_index & 1) == 0 && color_index != 0; j++)
            color_index >>= 1;
        comp_shift[i] = j;
        /* Determine the bit count for the colorant */
        for (j = 0; color_index != 0; j++) {
            if ((color_index & 1) == 0) /* check for non-consecutive bits */
                return;
            color_index >>= 1;
        }
        comp_bits[i] = j;
        /*
         * We could verify that the bit count matches the dither_grays or
         * dither_colors values, but this is not really required unless we
         * are halftoning.  Thus we are allowing for non equal colorant sizes.
         */
        /* Check for overlap with other colorant if they are all maxed */
        for (j = 0; j < num_components; j++)
            colorants[j] = gx_max_color_value;
        colorants[i] = 0;
        color_index = dev_proc(dev, encode_color)(dev, colorants);
        if (color_index & comp_mask[i])	/* Check for overlapping bits */
            return;
    }
    /* If we get to here then the device is very likely to be separable. */
    pinfo->separable_and_linear = GX_CINFO_SEP_LIN;
    for (i = 0; i < num_components; i++) {
        pinfo->comp_shift[i] = comp_shift[i];
        pinfo->comp_bits[i] = comp_bits[i];
        pinfo->comp_mask[i] = comp_mask[i];
    }
    /*
     * The 'gray_index' value allows one colorant to have a different number
     * of shades from the remainder.  Since the default macros only guess at
     * an appropriate value, we are setting its value based upon the data that
     * we just determined.  Note:  In some cases the macros set max_gray to 0
     * and dither_grays to 1.  This is not valid so ignore this case.
     */
    for (i = 0; i < num_components; i++) {
        int dither = 1 << comp_bits[i];

        if (pinfo->dither_grays != 1 && dither == pinfo->dither_grays) {
            pinfo->gray_index = i;
            break;
        }
    }
}