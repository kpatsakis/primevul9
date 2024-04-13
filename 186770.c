load_fontdata(
    XOC		oc,
    FontData	font_data,
    int		font_data_num)
{
    Display	*dpy = oc->core.om->core.display;
    FontData	fd = font_data;

    if(font_data == NULL) return(True);
    for( ; font_data_num-- ; fd++) {
	if(fd->xlfd_name != (char *) NULL && fd->font == NULL) {
	    fd->font = XLoadQueryFont(dpy, fd->xlfd_name);
	    if (fd->font == NULL){
		return False;
	    }
	}
    }
    return True;
}