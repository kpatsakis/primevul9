read_EncodingInfo(
    int count,
    char **value)
{
    FontData font_data,ret;
    char *buf, *bufptr,*scp;
    int len, i;
    font_data = Xcalloc(count, sizeof(FontDataRec));
    if (font_data == NULL)
        return NULL;

    ret = font_data;
    for (i = 0; i < count; i++, font_data++) {
/*
        strcpy(buf, *value++);
*/
	buf = *value; value++;
        if ((bufptr = strchr(buf, ':'))) {
	    len = (int)(bufptr - buf);
            bufptr++ ;
	} else
            len = strlen(buf);
        font_data->name = Xmalloc(len + 1);
        if (font_data->name == NULL) {
            free_fontdataOM(ret, i + 1);
            Xfree(ret);
            return NULL;
	}
        strncpy(font_data->name, buf,len);
	font_data->name[len] = 0;
        if (bufptr && _XlcCompareISOLatin1(bufptr, "GL") == 0)
            font_data->side = XlcGL;
        else if (bufptr && _XlcCompareISOLatin1(bufptr, "GR") == 0)
            font_data->side = XlcGR;
        else
            font_data->side = XlcGLGR;

        if (bufptr && (scp = strchr(bufptr, '['))){
            font_data->scopes = _XlcParse_scopemaps(scp,&(font_data->scopes_num));
        }
    }
    return(ret);
}