get_font_name(
    XOC oc,
    char *pattern)
{
    char **list, *name;
    int count = 0;

    list = XListFonts(oc->core.om->core.display, pattern, 1, &count);
    if (list == NULL)
	return NULL;

    name = strdup(*list);

    XFreeFontNames(list);

    return name;
}