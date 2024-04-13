epson_map_color_rgb(gx_device * dev, gx_color_index color,
                    gx_color_value prgb[3])
{
#define c1 gx_max_color_value
    if (gx_device_has_color(dev))
        switch ((ushort) color ^ 7) {
            case BLACK:
                prgb[0] = 0;
                prgb[1] = 0;
                prgb[2] = 0;
                break;
            case VIOLET:
                prgb[0] = 0;
                prgb[1] = 0;
                prgb[2] = c1;
                break;
            case GREEN:
                prgb[0] = 0;
                prgb[1] = c1;
                prgb[2] = 0;
                break;
            case CYAN:
                prgb[0] = 0;
                prgb[1] = c1;
                prgb[2] = c1;
                break;
            case RED:
                prgb[0] = c1;
                prgb[1] = 0;
                prgb[2] = 0;
                break;
            case MAGENTA:
                prgb[0] = c1;
                prgb[1] = 0;
                prgb[2] = c1;
                break;
            case YELLOW:
                prgb[0] = c1;
                prgb[1] = c1;
                prgb[2] = 0;
                break;
            case WHITE:
                prgb[0] = c1;
                prgb[1] = c1;
                prgb[2] = c1;
                break;
    } else
        return gx_default_map_color_rgb(dev, color, prgb);
    return 0;
}