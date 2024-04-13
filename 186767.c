create_om(
    XLCd lcd,
    Display *dpy,
    XrmDatabase rdb,
    _Xconst char *res_name,
    _Xconst char *res_class)
{
    XOM om;

    om = Xcalloc(1, sizeof(XOMGenericRec));
    if (om == NULL)
	return (XOM) NULL;

    om->methods = &methods;
    om->core.lcd = lcd;
    om->core.display = dpy;
    om->core.rdb = rdb;
    if (res_name) {
	om->core.res_name = strdup(res_name);
	if (om->core.res_name == NULL)
	    goto err;
    }
    if (res_class) {
	om->core.res_class = strdup(res_class);
	if (om->core.res_class == NULL)
	    goto err;
    }

    if (om_resources[0].xrm_name == NULLQUARK)
	_XlcCompileResourceList(om_resources, XlcNumber(om_resources));

    om->core.resources = om_resources;
    om->core.num_resources = XlcNumber(om_resources);

    return om;

err:
    close_om(om);

    return (XOM) NULL;
}