epson_map_rgb_color(gx_device * dev, const gx_color_value cv[])
{

    gx_color_value r = cv[0];
    gx_color_value g = cv[1];
    gx_color_value b = cv[2];

    if (gx_device_has_color(dev))
/* use ^7 so WHITE is 0 for internal calculations */
        return (gx_color_index) rgb_color[r >> cv_shift][g >> cv_shift][b >> cv_shift] ^ 7;
    else
        return gx_default_map_rgb_color(dev, cv);
}