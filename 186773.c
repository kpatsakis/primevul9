free_fontdataOC(
    Display	*dpy,
    FontData	font_data,
    int		font_data_count)
{
    for( ; font_data_count-- ; font_data++) {
	if(font_data->xlfd_name){
	    Xfree(font_data->xlfd_name);
	    font_data->xlfd_name = NULL;
	}
	if(font_data->font){				/* ADD 1996.01.7 */
	    if(font_data->font->fid)			/* Add 1996.01.23 */
		XFreeFont(dpy,font_data->font);		/* ADD 1996.01.7 */
	    else					/* Add 1996.01.23 */
		XFreeFontInfo(NULL, font_data->font, 1);/* Add 1996.01.23 */
	    font_data->font = NULL;
	}
/*
 * font_data->name and font_data->scopes belong to the OM not OC.
 * To save space this data is shared between OM and OC. We are
 * not allowed to free it here.
 * It has been moved to free_fontdataOM()
 */
/*
	if(font_data->scopes){
	    Xfree(font_data->scopes);
	    font_data->scopes = NULL;
	}
	if(font_data->name){
	    Xfree(font_data->name);
	    font_data->name = NULL;
	}
*/
    }
}