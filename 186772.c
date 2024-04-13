set_missing_list(
    XOC oc)
{
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set;
    char **charset_list, *charset_buf;
    int	count, length, font_set_num;
    int result = 1;

    font_set = gen->font_set;
    font_set_num = gen->font_set_num;
    count = length = 0;

    for ( ; font_set_num-- > 0; font_set++) {
	if (font_set->info || font_set->font) {
	    continue;
	}

	/* Change 1996.01.23 start */
	if(font_set->font_data_count <= 0 ||
	   font_set->font_data == (FontData)NULL) {
	    if(font_set->substitute_num <= 0 ||
	       font_set->substitute == (FontData)NULL) {
		if(font_set->charset_list != NULL){
		 length +=
		  strlen(font_set->charset_list[0]->encoding_name) + 1;
		} else {
		  length += 1;
		}
	    } else {
		length += strlen(font_set->substitute->name) + 1;
	    }
	} else {
	    length += strlen(font_set->font_data->name) + 1;
	}
	/* Change 1996.01.23 end */
	count++;
    }

    if (count < 1) {
	return True;
    }

    charset_list = Xmalloc(sizeof(char *) * count);
    if (charset_list == NULL) {
	return False;
    }

    charset_buf = Xmalloc(length);
    if (charset_buf == NULL) {
	Xfree(charset_list);
	return False;
    }

    oc->core.missing_list.charset_list = charset_list;
    oc->core.missing_list.charset_count = count;

    font_set = gen->font_set;
    font_set_num = gen->font_set_num;

    for ( ; font_set_num-- > 0; font_set++) {
	if (font_set->info || font_set->font) {
	    continue;
	}

	/* Change 1996.01.23 start */
	if(font_set->font_data_count <= 0 ||
	   font_set->font_data == (FontData)NULL) {
	    if(font_set->substitute_num <= 0 ||
	       font_set->substitute == (FontData)NULL) {
		if(font_set->charset_list != NULL){
		 strcpy(charset_buf,
			font_set->charset_list[0]->encoding_name);
		} else {
		 strcpy(charset_buf, "");
		}
		result = 0;
	    } else {
		strcpy(charset_buf, font_set->substitute->name);
	    }
	} else {
	    strcpy(charset_buf, font_set->font_data->name);
	}
	/* Change 1996.01.23 end */
	*charset_list++ = charset_buf;
	charset_buf += strlen(charset_buf) + 1;
    }

    if(result == 0) {
	return(False);
    }

    return True;
}