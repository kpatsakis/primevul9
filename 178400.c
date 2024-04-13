dvi_document_do_color_special (DviContext *dvi, const char *prefix, const char *arg)
{
        if (strncmp (arg, "pop", 3) == 0) {
                mdvi_pop_color (dvi);
        } else if (strncmp (arg, "push", 4) == 0) {
                /* Find color source: Named, CMYK or RGB */
                const char *tmp = arg + 4;
		
                while (isspace (*tmp)) tmp++;

                if (!strncmp ("rgb", tmp, 3)) {
			gdouble rgb[3];
                        guchar red, green, blue;

			parse_color (tmp + 4, rgb, 3);
			
                        red = 255 * rgb[0];
                        green = 255 * rgb[1];
                        blue = 255 * rgb[2];

                        mdvi_push_color (dvi, RGB2ULONG (red, green, blue), 0xFFFFFFFF);
                } else if (!strncmp ("hsb", tmp, 4)) {
                        gdouble hsb[3];
                        guchar red, green, blue;

			parse_color (tmp + 4, hsb, 3);
			
                        if (hsb2rgb (hsb[0], hsb[1], hsb[2], &red, &green, &blue))
                                mdvi_push_color (dvi, RGB2ULONG (red, green, blue), 0xFFFFFFFF);
                } else if (!strncmp ("cmyk", tmp, 4)) {
			gdouble cmyk[4];
                        double r, g, b;
			guchar red, green, blue;
			
			parse_color (tmp + 5, cmyk, 4);

                        r = 1.0 - cmyk[0] - cmyk[3];
                        if (r < 0.0)
                                r = 0.0;
                        g = 1.0 - cmyk[1] - cmyk[3];
                        if (g < 0.0)
                                g = 0.0;
                        b = 1.0 - cmyk[2] - cmyk[3];
                        if (b < 0.0)
                                b = 0.0;

			red = r * 255 + 0.5;
			green = g * 255 + 0.5;
			blue = b * 255 + 0.5;
			
                        mdvi_push_color (dvi, RGB2ULONG (red, green, blue), 0xFFFFFFFF);
		} else if (!strncmp ("gray ", tmp, 5)) {
			gdouble gray;
			guchar rgb;

			parse_color (tmp + 5, &gray, 1);

			rgb = gray * 255 + 0.5;

			mdvi_push_color (dvi, RGB2ULONG (rgb, rgb, rgb), 0xFFFFFFFF);
                } else {
                        GdkColor color;
			
                        if (gdk_color_parse (tmp, &color)) {
				guchar red, green, blue;

				red = color.red * 255 / 65535.;
				green = color.green * 255 / 65535.;
				blue = color.blue * 255 / 65535.;

                                mdvi_push_color (dvi, RGB2ULONG (red, green, blue), 0xFFFFFFFF);
			}
                }
        }
}