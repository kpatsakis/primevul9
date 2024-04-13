gui_bg_default(void)
{
    if (gui_get_lightness(gui.back_pixel) < 127)
	return (char_u *)"dark";
    return (char_u *)"light";
}