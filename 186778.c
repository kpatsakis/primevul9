parse_all_name(
    XOC		oc,
    FontData	font_data,
    char	*pattern)
{

#ifdef OLDCODE
    if(is_match_charset(font_data, pattern) != True)
 	return False;

    font_data->xlfd_name = strdup(pattern);
    if(font_data->xlfd_name == NULL)
	return (-1);

    return True;
#else  /* OLDCODE */
    Display *dpy = oc->core.om->core.display;
    char **fn_list = NULL, *prop_fname = NULL;
    int list_num;
    XFontStruct *fs_list;
    if(is_match_charset(font_data, pattern) != True) {
	/*
	 * pattern should not contain any wildcard (execpt '?')
	 * this was probably added to make this case insensitive.
	 */
	if ((fn_list = XListFontsWithInfo(dpy, pattern,
				      MAXFONTS,
				      &list_num, &fs_list)) == NULL) {
            return False;
        }
	/* shouldn't we loop here ? */
        else if ((prop_fname = get_prop_name(dpy, fs_list)) == NULL) {
            XFreeFontInfo(fn_list, fs_list, list_num);
            return False;
        }
        else if ((is_match_charset(font_data, prop_fname) != True)) {
            XFree(prop_fname);
            XFreeFontInfo(fn_list, fs_list, list_num);
            return False;
        }
        else {
	    font_data->xlfd_name = prop_fname;
            XFreeFontInfo(fn_list, fs_list, list_num);
            return True;
        }
    }

    font_data->xlfd_name = strdup(pattern);
    if(font_data->xlfd_name == NULL)
	return (-1);

    return True;
#endif /* OLDCODE */
}