parse_fontdata(
    XOC		 oc,
    FontSet      font_set,
    FontData	 font_data,
    int		 font_data_count,
    char	 **name_list,
    int		 name_list_count,
    ClassType	 class,
    FontDataRec *font_data_return)
{

    char	**cur_name_list = name_list;
    char	*font_name      = (char *) NULL;
    char	*pattern        = (char *) NULL;
    int		found_num       = 0, ret = 0;
    int		count           = name_list_count;

    if(name_list == NULL || count <= 0) {
	return False;
    }

    if(font_data == NULL || font_data_count <= 0) {
	return False;
    }

    /* Loop through each font encoding defined in the "font_data" FontSet. */
    for ( ; font_data_count-- > 0; font_data++) {
	Bool	is_found = False;
	font_name = (char *) NULL;
	count = name_list_count;
	cur_name_list = name_list;

       /*
	* Loop through each font specified by the user
	* in the call to XCreateFontset().
	*/
	while (count-- > 0) {
            pattern = *cur_name_list++;
	    if (pattern == NULL || *pattern == '\0')
		continue;
#ifdef FONTDEBUG
		fprintf(stderr,"Font pattern: %s %s\n",
		pattern,font_data->name);
#endif

	    /*
	     * If the current font is fully specified (i.e., the
	     * xlfd contains no wildcards) and the font exists on
	     * the X Server, we have a match.
	     */
	    if (strchr(pattern, '*') == NULL &&
		(font_name = get_font_name(oc, pattern))) {
               /*
		* Find the full xlfd name for this font. If the font is
		* already in xlfd format, it is simply returned.  If the
		* font is an alias for another font, the xlfd of the
		* aliased font is returned.
		*/
		ret = parse_all_name(oc, font_data, font_name);
		Xfree(font_name);

                if (ret == -1)    return -1;
	        if (ret == False) continue;
               /*
		* Since there was an exact match of a fully-specified font
		* or a font alias, we can return now since the desired font
		* was found for the current font encoding for this FontSet.
		*
		* Previous implementations of this algorithm would
		* not return here. Instead, they continued searching
		* through the font encodings for this FontSet. The side-effect
		* of that behavior is you may return a "substitute" match
		* instead of an "exact" match.  We believe there should be a
		* preference on exact matches.  Therefore, as soon as we
		* find one, we bail.
		*
		* Also, previous implementations seemed to think it was
		* important to find either a primary or substitute font
		* for each Font encoding in the FontSet before returning an
		* acceptable font.  We don't believe this is necessary.
		* All the client cares about is finding a reasonable font
		* for what was passed in.  If we find an exact match,
		* there's no reason to look any further.
		*
		* -- jjw/pma (HP)
		*/
		if (font_data_return) {
		    font_data_return->xlfd_name = strdup(font_data->xlfd_name);
		    if (!font_data_return->xlfd_name) return -1;

		    font_data_return->side      = font_data->side;
		}
#ifdef FONTDEBUG
		fprintf(stderr,"XLFD name: %s\n",font_data->xlfd_name);
#endif

		return True;
	    }
	    /*
	     * If the font name is not fully specified
	     * (i.e., it has wildcards), we have more work to do.
	     * See the comments in parse_omit_name()
	     * for the list of things to do.
	     */
	    ret = parse_omit_name(oc, font_data, pattern);

            if (ret == -1)    return -1;
	    if (ret == False) continue;

           /*
	    * A font which matched the wild-carded specification was found.
	    * Only update the return data if a font has not yet been found.
	    * This maintains the convention that FontSets listed higher in
	    * a CodeSet in the Locale Database have higher priority than
	    * those FontSets listed lower in the CodeSet.  In the following
	    * example:
	    *
	    * fs1 {
	    *        charset     HP-JIS:GR
	    *        font        JISX0208.1990-0:GL;\
	    *                    JISX0208.1990-1:GR;\
	    *                    JISX0208.1983-0:GL;\
	    *                    JISX0208.1983-1:GR
	    * }
	    *
	    * a font found in the JISX0208.1990-0 FontSet will have a
	    * higher priority than a font found in the JISX0208.1983-0
	    * FontSet.
	    */
	    if (font_data_return && font_data_return->xlfd_name == NULL) {

#ifdef FONTDEBUG
		fprintf(stderr,"XLFD name: %s\n",font_data->xlfd_name);
#endif
		font_data_return->xlfd_name = strdup(font_data->xlfd_name);
                if (!font_data_return->xlfd_name) return -1;

	        font_data_return->side      = font_data->side;
	    }

	    found_num++;
	    is_found = True;

	    break;
	}

	switch(class) {
	  case C_PRIMARY:
	       if(is_found == False) {
		 /*
		  * Did not find a font for the current FontSet.  Check the
		  * FontSet's "substitute" font for a match.  If we find a
		  * match, we'll keep searching in hopes of finding an exact
		  * match later down the FontSet list.
		  *
		  * when we return and we have found a font font_data_return
		  * contains the first (ie. best) match no matter if this
		  * is a C_PRIMARY or a C_SUBSTITUTE font
		  */
		  ret = parse_fontdata(oc, font_set, font_set->substitute,
				       font_set->substitute_num, name_list,
				       name_list_count, C_SUBSTITUTE,
				       font_data_return);
                  if (ret == -1)    return -1;
		  if (ret == False) continue;

		  found_num++;
		  is_found = True;
               }
#ifdef TESTVERSION
	       else
		   return True;
#endif
	       break;

	  case C_SUBSTITUTE:
	  case C_VMAP:
	       if(is_found == True)
		  return True;
	       break;

	  case C_VROTATE:
	       if(is_found == True) {
		  char	*rotate_name;

		  if((rotate_name = get_rotate_fontname(font_data->xlfd_name))
		     != NULL) {
		      Xfree(font_data->xlfd_name);
		      font_data->xlfd_name = rotate_name;

		      return True;
		  }
		  Xfree(font_data->xlfd_name);
		  font_data->xlfd_name = NULL;
		  return False;
	       }
	       break;
	}
    }

    if(class == C_PRIMARY && found_num >= 1)
	return True;

    return False;
}