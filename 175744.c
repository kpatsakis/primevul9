void check_device_compatible_encoding(gx_device *dev)
{
    gx_device_color_info * pinfo = &(dev->color_info);
    int num_components = pinfo->num_components;
    gx_color_index mul, color_index;
    int i, j;
    gx_color_value colorants[GX_DEVICE_COLOR_MAX_COMPONENTS];

    if (pinfo->separable_and_linear == GX_CINFO_UNKNOWN_SEP_LIN)
        check_device_separable(dev);
    if (pinfo->separable_and_linear != GX_CINFO_SEP_LIN)
        return;

    if (dev_proc(dev, ret_devn_params)(dev) != NULL) {
        /* We know all devn devices are compatible. */
        pinfo->separable_and_linear = GX_CINFO_SEP_LIN_STANDARD;
        return;
    }

    /* Do the superficial quick checks */
    for (i = 0; i < num_components; i++) {
        int shift = (num_components-1-i)*8;
        if (pinfo->comp_shift[i] != shift)
            goto bad;
        if (pinfo->comp_bits[i] != 8)
            goto bad;
        if (pinfo->comp_mask[i] != ((gx_color_index)255)<<shift)
            goto bad;
    }

    /* OK, now we are going to be slower. */
    mul = 0;
    for (i = 0; i < num_components; i++) {
        mul = (mul<<8) | 1;
    }
    for (i = 0; i < 255; i++) {
        for (j = 0; j < num_components; j++)
            colorants[j] = i*257;
        color_index = dev_proc(dev, encode_color)(dev, colorants);
        if (color_index != i*mul && (i*mul != gx_no_color_index_value))
            goto bad;
    }
    /* If we reach here, then every value matched, except possibly the last one.
     * We'll allow that to differ just in the lowest bits. */
    if ((color_index | mul) != 255*mul)
        goto bad;

    pinfo->separable_and_linear = GX_CINFO_SEP_LIN_STANDARD;
    return;
bad:
    pinfo->separable_and_linear = GX_CINFO_SEP_LIN_NON_STANDARD;
}