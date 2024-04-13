(*get_decode_color(gx_device * dev))(gx_device *, gx_color_index, gx_color_value *)
{
    /* if a method has already been provided, use it */
    if (dev_proc(dev, decode_color) != 0)
        return dev_proc(dev, decode_color);

    /*
     * If a map_color_rgb method has been provided, we may be able to use it.
     * Currently this will always be the case, as a default value will be
     * provided this method. While this default may not be correct, we are not
     * introducing any new errors by using it.
     */
    if (dev_proc(dev, map_color_rgb) != 0) {

        /* if the device has a DeviceRGB color model, use map_color_rgb */
        if (is_like_DeviceRGB(dev))
            return dev_proc(dev, map_color_rgb);

        /* If separable ande linear then use default */
        if (colors_are_separable_and_linear(&dev->color_info))
            return &gx_default_decode_color;

        /* gray devices can be handled based on their polarity */
        if ( dev->color_info.num_components == 1 &&
             dev->color_info.gray_index == 0       )
            return dev->color_info.polarity == GX_CINFO_POLARITY_ADDITIVE
                       ? &gx_default_1_add_decode_color
                       : &gx_default_1_sub_decode_color;

        /*
         * There is no accurate way to decode colors for cmyk devices
         * using the map_color_rgb procedure. Unfortunately, this cases
         * arises with some frequency, so it is useful not to generate an
         * error in this case. The mechanism below assumes full undercolor
         * removal and black generation, which may not be accurate but are
         * the  best that can be done in the general case in the absence of
         * other information.
         *
         * As a hack to handle certain common devices, if the map_rgb_color
         * routine is cmyk_1bit_map_color_rgb, we provide a direct one-bit
         * decoder.
         */
        if (is_like_DeviceCMYK(dev)) {
            if (dev_proc(dev, map_color_rgb) == cmyk_1bit_map_color_rgb)
                return &gx_1bit_cmyk_decode_color;
            else
                return &gx_default_cmyk_decode_color;
        }
    }

    /*
     * The separable and linear case will already have been handled by
     * code in gx_device_fill_in_procs, so at this point we can only hope
     * the device doesn't use the decode_color method.
     */
    if (colors_are_separable_and_linear(&dev->color_info))
        return &gx_default_decode_color;
    else
        return &gx_error_decode_color;
}