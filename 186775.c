load_font(
    XOC oc)
{
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set = gen->font_set;
    int num = gen->font_set_num;

    for ( ; num-- > 0; font_set++) {
	if (font_set->font_name == NULL)
	    continue;

        if (load_fontset_data (oc, font_set) != True)
	    return False;
#ifndef TESTVERSION
	if(load_fontdata(oc, font_set->font_data,
			 font_set->font_data_count) != True)
	    return False;

	if(load_fontdata(oc, font_set->substitute,
			 font_set->substitute_num) != True)
	    return False;
#endif

/* Add 1996.05.20 */
        if( oc->core.orientation == XOMOrientation_TTB_RTL ||
            oc->core.orientation == XOMOrientation_TTB_LTR ){
	    if (font_set->vpart_initialize == 0) {
	       load_fontdata(oc, font_set->vmap, font_set->vmap_num);
	       load_fontdata(oc, (FontData) font_set->vrotate,
			 font_set->vrotate_num);
                font_set->vpart_initialize = 1;
	    }
        }

	if (font_set->font->min_byte1 || font_set->font->max_byte1)
	    font_set->is_xchar2b = True;
	else
	    font_set->is_xchar2b = False;
    }

    return True;
}