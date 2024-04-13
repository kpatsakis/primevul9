get_rotate_fontname(
    char *font_name)
{
    char *pattern = NULL, *ptr = NULL;
    char *fields[CHARSET_ENCODING_FIELD];
    char str_pixel[32], str_point[4];
    char *rotate_font_ptr = NULL;
    int pixel_size = 0;
    int field_num = 0, len = 0;

    if(font_name == (char *) NULL || (len = strlen(font_name)) <= 0
       || len > XLFD_MAX_LEN)
	return NULL;

    pattern = strdup(font_name);
    if(!pattern)
	return NULL;

    memset(fields, 0, sizeof(char *) * 14);
    ptr = pattern;
    while(isspace(*ptr)) {
	ptr++;
    }
    if(*ptr == '-')
	ptr++;

    for(field_num = 0 ; field_num < CHARSET_ENCODING_FIELD && ptr && *ptr ;
			ptr++, field_num++) {
	fields[field_num] = ptr;

	if((ptr = strchr(ptr, '-'))) {
	    *ptr = '\0';
	} else {
	    field_num++;	/* Count last field */
	    break;
	}
    }

    if(field_num < CHARSET_ENCODING_FIELD)
	goto free_pattern;

    /* Pixel Size field : fields[6] */
    for(ptr = fields[PIXEL_SIZE_FIELD - 1] ; ptr && *ptr; ptr++) {
	if(!isdigit(*ptr)) {
	    if(*ptr == '['){ /* 960730 */
	        strcpy(pattern, font_name);
		return(pattern);
	    }
	    goto free_pattern;
	}
    }
    pixel_size = atoi(fields[PIXEL_SIZE_FIELD - 1]);
    snprintf(str_pixel, sizeof(str_pixel),
	     "[ 0 ~%d %d 0 ]", pixel_size, pixel_size);
    fields[6] = str_pixel;

    /* Point Size field : fields[7] */
    strcpy(str_point, "*");
    fields[POINT_SIZE_FIELD - 1] = str_point;

    len = 0;
    for (field_num = 0; field_num < CHARSET_ENCODING_FIELD &&
			fields[field_num]; field_num++) {
	len += 1 + strlen(fields[field_num]);
    }

    /* Max XLFD length is 255 */
    if (len > XLFD_MAX_LEN)
	goto free_pattern;

    rotate_font_ptr = Xmalloc(len + 1);
    if(!rotate_font_ptr)
	goto free_pattern;

    rotate_font_ptr[0] = '\0';

    for(field_num = 0 ; field_num < CHARSET_ENCODING_FIELD &&
			fields[field_num] ; field_num++) {
	strcat(rotate_font_ptr, "-");
	strcat(rotate_font_ptr, fields[field_num]);
    }

free_pattern:
    Xfree(pattern);

    return rotate_font_ptr;
}