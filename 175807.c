gx_default_1_sub_decode_color(
    gx_device *     dev,
    gx_color_index  color,
    gx_color_value  cv[1] )
{
    gx_color_value  rgb[3];
    int             code = dev_proc(dev, map_color_rgb)(dev, color, rgb);

    cv[0] = gx_max_color_value - rgb[0];
    return code;
}