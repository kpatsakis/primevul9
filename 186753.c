get_prop_name(
    Display *dpy,
    XFontStruct	*fs)
{
    unsigned long fp;

    if (XGetFontProperty(fs, XA_FONT, &fp))
	return XGetAtomName(dpy, fp);

    return (char *) NULL;
}