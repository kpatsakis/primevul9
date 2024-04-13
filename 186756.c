set_oc_values(
    XOC oc,
    XlcArgList args,
    int num_args)
{
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set = gen->font_set;
    char *ret;
    int num = gen->font_set_num;

    if (oc->core.resources == NULL)
	return NULL;

    ret = _XlcSetValues((XPointer) oc, oc->core.resources,
			oc->core.num_resources, args, num_args, XlcSetMask);
    if(ret != NULL){
	return(ret);
    } else {
	for ( ; num-- > 0; font_set++) {
	    if (font_set->font_name == NULL)
	        continue;
	    if (font_set->vpart_initialize != 0)
	        continue;
	    if( oc->core.orientation == XOMOrientation_TTB_RTL ||
		oc->core.orientation == XOMOrientation_TTB_LTR ){
	    	load_fontdata(oc, font_set->vmap, font_set->vmap_num);
		load_fontdata(oc, (FontData) font_set->vrotate,
			    font_set->vrotate_num);
		font_set->vpart_initialize = 1;
	    }
	}
	return(NULL);
    }
}