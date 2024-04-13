static void read_vw(
    XLCd    lcd,
    OMData  font_set,
    int     num)
{
    char **value, buf[BUFSIZ];
    int count;

    snprintf(buf, sizeof(buf), "fs%d.font.vertical_map", num);
    _XlcGetResource(lcd, "XLC_FONTSET", buf, &value, &count);
    if (count > 0){
        _XlcDbg_printValue(buf,value,count);
        font_set->vmap_num = count;
        font_set->vmap = read_EncodingInfo(count,value);
    }

    snprintf(buf, sizeof(buf), "fs%d.font.vertical_rotate", num);
    _XlcGetResource(lcd, "XLC_FONTSET", buf, &value, &count);
    if (count > 0){
        _XlcDbg_printValue(buf,value,count);
        font_set->vrotate = read_vrotate(count,value,&(font_set->vrotate_type),
				&(font_set->vrotate_num));
    }
}