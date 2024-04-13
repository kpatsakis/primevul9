termgui_get_color(char_u *name)
{
    guicolor_T	t;

    if (*name == NUL)
	return INVALCOLOR;
    t = termgui_mch_get_color(name);

    if (t == INVALCOLOR)
	semsg(_(e_cannot_allocate_color_str), name);
    return t;
}