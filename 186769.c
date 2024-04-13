destroy_oc(
    XOC oc)
{
    Display *dpy = oc->core.om->core.display;
    XOCGenericPart *gen = XOC_GENERIC(oc);

    if (gen->mbs_to_cs)
	_XlcCloseConverter(gen->mbs_to_cs);

    if (gen->wcs_to_cs)
	_XlcCloseConverter(gen->wcs_to_cs);

    if (gen->utf8_to_cs)
	_XlcCloseConverter(gen->utf8_to_cs);

/* For VW/UDC start */ /* Change 1996.01.8 */
    destroy_fontdata(gen,dpy);
/*
*/
/* For VW/UDC end */

    Xfree(oc->core.base_name_list);
    XFreeStringList(oc->core.font_info.font_name_list);
    Xfree(oc->core.font_info.font_struct_list);
    XFreeStringList(oc->core.missing_list.charset_list);

#ifdef notdef

    Xfree(oc->core.res_name);
    Xfree(oc->core.res_class);
#endif

    Xfree(oc);
}