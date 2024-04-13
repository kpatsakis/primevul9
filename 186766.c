parse_fontname(
    XOC oc)
{
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet     font_set;
    FontDataRec font_data_return;
    char *base_name, **name_list;
    int font_set_num = 0;
    int found_num = 0;
    int count = 0;
    int	ret;
    int i;

    name_list = _XParseBaseFontNameList(oc->core.base_name_list, &count);
    if (name_list == NULL)
	return -1;

    font_set = gen->font_set;
    font_set_num = gen->font_set_num;

   /* Loop through all of the CharSets defined in the Locale
    * database for the current Locale.
    */
    for( ; font_set_num-- > 0 ; font_set++) {
	if(font_set->font_name)
	    continue;

	if(font_set->font_data_count > 0) {

           /*
	    * If there are a non-zero number of FontSets defined
	    * for this CharSet.
            * Try to find a font for this CharSet.  If we find an
	    * acceptable font, we save the information for return
	    * to the client.  If we do not find an acceptable font,
	    * a "missing_charset" will be reported to the client
	    * for this CharSet.
	    */
	    font_data_return.xlfd_name  = NULL;
	    font_data_return.side       = XlcUnknown;

	    ret = parse_fontdata(oc, font_set, font_set->font_data,
				 font_set->font_data_count,
				 name_list, count, C_PRIMARY,
				 &font_data_return);
	    if(ret == -1) {
		goto err;
	    } else if(ret == True) {
		/*
		 * We can't just loop through fontset->font_data to
		 * find the first (ie. best) match: parse_fontdata
		 * will try a substitute font if no primary one could
		 * be matched. It returns the required information in
		 * font_data_return.
		 */
		font_set->font_name = strdup(font_data_return.xlfd_name);
		if(font_set->font_name == (char *) NULL)
		    goto err;

		font_set->side = font_data_return.side;

                Xfree (font_data_return.xlfd_name);
                font_data_return.xlfd_name = NULL;

		if(parse_vw(oc, font_set, name_list, count) == -1)
		    goto err;
		found_num++;
	    }

	} else if(font_set->substitute_num > 0) {
           /*
	    * If there are no FontSets defined for this
	    * CharSet.  We can only find "substitute" fonts.
	    */
	    ret = parse_fontdata(oc, font_set, font_set->substitute,
				 font_set->substitute_num,
				 name_list, count, C_SUBSTITUTE, NULL);
	    if(ret == -1) {
		goto err;
	    } else if(ret == True) {
		for(i=0;i<font_set->substitute_num;i++){
		    if(font_set->substitute[i].xlfd_name != NULL){
			break;
		    }
		}
		font_set->font_name = strdup(font_set->substitute[i].xlfd_name);
		if(font_set->font_name == (char *) NULL)
		    goto err;

		font_set->side = font_set->substitute[i].side;
		if(parse_vw(oc, font_set, name_list, count) == -1)
		    goto err;

		found_num++;
	    }
	}
    }

    base_name = strdup(oc->core.base_name_list);
    if (base_name == NULL)
	goto err;

    oc->core.base_name_list = base_name;

    XFreeStringList(name_list);

    return found_num;

err:
    XFreeStringList(name_list);
    /* Prevent this from being freed twice */
    oc->core.base_name_list = NULL;

    return -1;
}