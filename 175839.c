gx_default_cmyk_decode_color(
    gx_device *     dev,
    gx_color_index  color,
    gx_color_value  cv[4] )
{
    /* The device may have been determined to be 'separable'. */
    if (colors_are_separable_and_linear(&dev->color_info))
        return gx_default_decode_color(dev, color, cv);
    else {
        int i, code = dev_proc(dev, map_color_rgb)(dev, color, cv);
        gx_color_value min_val = gx_max_color_value;

        for (i = 0; i < 3; i++) {
            if ((cv[i] = gx_max_color_value - cv[i]) < min_val)
                min_val = cv[i];
        }
        for (i = 0; i < 3; i++)
            cv[i] -= min_val;
        cv[3] = min_val;

        return code;
    }
}