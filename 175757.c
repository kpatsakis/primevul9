gx_1bit_cmyk_decode_color(
    gx_device *     dev,
    gx_color_index  color,
    gx_color_value  cv[4] )
{
    cv[0] = ((color & 0x8) != 0 ? gx_max_color_value : 0);
    cv[1] = ((color & 0x4) != 0 ? gx_max_color_value : 0);
    cv[2] = ((color & 0x2) != 0 ? gx_max_color_value : 0);
    cv[3] = ((color & 0x1) != 0 ? gx_max_color_value : 0);
    return 0;
}