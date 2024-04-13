term_bg_rgb_color(guicolor_T rgb)
{
    if (rgb != INVALCOLOR)
	term_rgb_color(T_8B, rgb);
}