init_core_part(
    XOC oc)
{
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set;
    int font_set_num;
    XFontStruct **font_struct_list;
    char **font_name_list, *font_name_buf;
    int	count, length;

    font_set = gen->font_set;
    font_set_num = gen->font_set_num;
    count = length = 0;

    for ( ; font_set_num-- > 0; font_set++) {
	if (font_set->font_name == NULL)
	    continue;

	length += strlen(font_set->font_name) + 1;

	count++;
    }
    if (count == 0)
        return False;

    font_struct_list = Xmalloc(sizeof(XFontStruct *) * count);
    if (font_struct_list == NULL)
	return False;

    font_name_list = Xmalloc(sizeof(char *) * count);
    if (font_name_list == NULL)
	goto err;

    font_name_buf = Xmalloc(length);
    if (font_name_buf == NULL)
	goto err;

    oc->core.font_info.num_font = count;
    oc->core.font_info.font_name_list = font_name_list;
    oc->core.font_info.font_struct_list = font_struct_list;

    font_set = gen->font_set;
    font_set_num = gen->font_set_num;

    for (count = 0; font_set_num-- > 0; font_set++) {
	if (font_set->font_name == NULL)
	    continue;

	font_set->id = count;
	if (font_set->font)
	    *font_struct_list++ = font_set->font;
	else
	    *font_struct_list++ = font_set->info;
	strcpy(font_name_buf, font_set->font_name);
	Xfree(font_set->font_name);
	*font_name_list++ = font_set->font_name = font_name_buf;
	font_name_buf += strlen(font_name_buf) + 1;

	count++;
    }

    set_fontset_extents(oc);

    return True;

err:

    Xfree(font_name_list);
    Xfree(font_struct_list);

    return False;
}