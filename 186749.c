close_om(
    XOM om)
{
    XOMGenericPart *gen = XOM_GENERIC(om);
    OMData data;
    int count;

    if ((data = gen->data)) {
	for (count = gen->data_num; count-- > 0; data++) {
		Xfree(data->charset_list);
		data->charset_list = NULL;
	
	    /* free font_data for om */
		free_fontdataOM(data->font_data,data->font_data_count);
		Xfree(data->font_data);
		data->font_data = NULL;

	    /* free substitute for om */
		free_fontdataOM(data->substitute,data->substitute_num);
		Xfree(data->substitute);
		data->substitute = NULL;

	    /* free vmap for om */
		free_fontdataOM(data->vmap,data->vmap_num);
		Xfree(data->vmap);
		data->vmap = NULL;

	    /* free vrotate for om */
		Xfree(data->vrotate);
		data->vrotate = NULL;
	}
	Xfree(gen->data);
	gen->data = NULL;
    }

	Xfree(gen->object_name);
	gen->object_name = NULL;

	Xfree(om->core.res_name);
	om->core.res_name = NULL;

	Xfree(om->core.res_class);
	om->core.res_class = NULL;

    if (om->core.required_charset.charset_list &&
	om->core.required_charset.charset_count > 0){
	XFreeStringList(om->core.required_charset.charset_list);
	om->core.required_charset.charset_list = NULL;
    } else {
	Xfree((char*)om->core.required_charset.charset_list);
	om->core.required_charset.charset_list = NULL;
    }

	Xfree(om->core.orientation_list.orientation);
	om->core.orientation_list.orientation = NULL;

    Xfree(om);

    return 1;
}