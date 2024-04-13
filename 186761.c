static void destroy_fontdata(
    XOCGenericPart *gen,
    Display *dpy)
{
    FontSet	font_set = (FontSet) NULL;
    int		font_set_num = 0;

    if (gen->font_set) {
	font_set = gen->font_set;
	font_set_num = gen->font_set_num;
	for( ; font_set_num-- ; font_set++) {
	    if (font_set->font) {
		if(font_set->font->fid)
		    XFreeFont(dpy,font_set->font);
		else
		    XFreeFontInfo(NULL, font_set->font, 1);
		font_set->font = NULL;
	    }
	    if(font_set->font_data) {
		if (font_set->info)
		    XFreeFontInfo(NULL, font_set->info, 1);
		free_fontdataOC(dpy,
			font_set->font_data, font_set->font_data_count);
		Xfree(font_set->font_data);
		font_set->font_data = NULL;
	    }
	    if(font_set->substitute) {
		free_fontdataOC(dpy,
			font_set->substitute, font_set->substitute_num);
		Xfree(font_set->substitute);
		font_set->substitute = NULL;
	    }
	    if(font_set->vmap) {
		free_fontdataOC(dpy,
			font_set->vmap, font_set->vmap_num);
		Xfree(font_set->vmap);
		font_set->vmap = NULL;
	    }
	    if(font_set->vrotate) {
		free_fontdataOC(dpy,
			(FontData)font_set->vrotate,
			      font_set->vrotate_num);
		Xfree(font_set->vrotate);
		font_set->vrotate = NULL;
	    }
	}
	Xfree(gen->font_set);
	gen->font_set = NULL;
    }
}