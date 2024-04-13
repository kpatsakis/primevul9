load_font_info(
    XOC oc)
{
    Display *dpy = oc->core.om->core.display;
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set = gen->font_set;
    char **fn_list;
    int fn_num, num = gen->font_set_num;

    for ( ; num-- > 0; font_set++) {
	if (font_set->font_name == NULL)
	    continue;

	if (font_set->info == NULL) {
	    fn_list = XListFontsWithInfo(dpy, font_set->font_name, 1, &fn_num,
					 &font_set->info);
	    if (font_set->info == NULL)
		return False;

	    XFreeFontNames(fn_list);
	}
    }

    return True;
}