_XomGenericOpenOM(XLCd lcd, Display *dpy, XrmDatabase rdb,
		  _Xconst char *res_name, _Xconst char *res_class)
{
    XOM om;

    om = create_om(lcd, dpy, rdb, res_name, res_class);
    if (om == NULL)
	return (XOM) NULL;

    if (init_om(om) == False)
	goto err;

    return om;

err:
    close_om(om);

    return (XOM) NULL;
}