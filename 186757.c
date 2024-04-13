set_fontset_extents(
    XOC oc)
{
    XRectangle *ink = &oc->core.font_set_extents.max_ink_extent;
    XRectangle *logical = &oc->core.font_set_extents.max_logical_extent;
    XFontStruct **font_list, *font;
    XCharStruct overall;
    int logical_ascent, logical_descent;
    int	num = oc->core.font_info.num_font;

    font_list = oc->core.font_info.font_struct_list;
    font = *font_list++;
    overall = font->max_bounds;
    overall.lbearing = font->min_bounds.lbearing;
    logical_ascent = font->ascent;
    logical_descent = font->descent;

    /* For Vertical Writing start */

    while (--num > 0) {
	font = *font_list++;
	check_fontset_extents(&overall, &logical_ascent, &logical_descent,
			      font);
    }

    {
	XOCGenericPart  *gen = XOC_GENERIC(oc);
	FontSet		font_set = gen->font_set;
	FontData	font_data;
	int		font_set_num = gen->font_set_num;
	int		font_data_count;

	for( ; font_set_num-- ; font_set++) {
	    if(font_set->vmap_num > 0) {
		font_data = font_set->vmap;
		font_data_count = font_set->vmap_num;
		for( ; font_data_count-- ; font_data++) {
		    if(font_data->font != NULL) {
			check_fontset_extents(&overall, &logical_ascent,
					      &logical_descent,
					      font_data->font);
		    }
		}
	    }

	    if(font_set->vrotate_num > 0 && font_set->vrotate != NULL) {
		font_data = (FontData) font_set->vrotate;
		font_data_count = font_set->vrotate_num;
		for( ; font_data_count-- ; font_data++) {
		    if(font_data->font != NULL) {
			check_fontset_extents(&overall, &logical_ascent,
					      &logical_descent,
					      font_data->font);
		    }
		}
	    }
	}
    }

    /* For Vertical Writing start */

    ink->x = overall.lbearing;
    ink->y = -(overall.ascent);
    ink->width = overall.rbearing - overall.lbearing;
    ink->height = overall.ascent + overall.descent;

    logical->x = 0;
    logical->y = -(logical_ascent);
    logical->width = overall.width;
    logical->height = logical_ascent + logical_descent;
}