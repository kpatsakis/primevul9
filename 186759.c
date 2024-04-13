init_fontset(
    XOC oc)
{
    XOCGenericPart *gen;
    FontSet font_set;
    OMData data;
    int count;

    count = XOM_GENERIC(oc->core.om)->data_num;
    data = XOM_GENERIC(oc->core.om)->data;

    font_set = Xcalloc(count, sizeof(FontSetRec));
    if (font_set == NULL)
	return False;

    gen = XOC_GENERIC(oc);
    gen->font_set_num = count;
    gen->font_set = font_set;

    for ( ; count-- > 0; data++, font_set++) {
	font_set->charset_count = data->charset_count;
	font_set->charset_list = data->charset_list;

	if((font_set->font_data = init_fontdata(data->font_data,
				  data->font_data_count)) == NULL)
	    goto err;
	font_set->font_data_count = data->font_data_count;
	if((font_set->substitute = init_fontdata(data->substitute,
				   data->substitute_num)) == NULL)
	    goto err;
	font_set->substitute_num = data->substitute_num;
	if((font_set->vmap = init_fontdata(data->vmap,
			     data->vmap_num)) == NULL)
	    goto err;
	font_set->vmap_num       = data->vmap_num;

	if(data->vrotate_type != VROTATE_NONE) {
	    /* A vrotate member is specified primary font data */
	    /* as initial value.                               */
	    if((font_set->vrotate = init_vrotate(data->font_data,
						 data->font_data_count,
						 data->vrotate_type,
						 data->vrotate,
						 data->vrotate_num)) == NULL)
		goto err;
	    font_set->vrotate_num = data->font_data_count;
	}
    }
    return True;

err:

    Xfree(font_set->font_data);
    Xfree(font_set->substitute);
    Xfree(font_set->vmap);
    Xfree(font_set->vrotate);
    Xfree(font_set);
    gen->font_set = (FontSet) NULL;
    gen->font_set_num = 0;
    return False;
}