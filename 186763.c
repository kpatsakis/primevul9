parse_omit_name(
    XOC		oc,
    FontData	font_data,
    char	*pattern)
{
    char*	last = (char *) NULL;
    char*	base_name;
    char	buf[XLFD_MAX_LEN + 1];
    int		length = 0;
    int		num_fields;
   /*
    * If the font specified by "pattern" is expandable to be
    * a member of "font_data"'s FontSet, we've found a match.
    */
    if(is_match_charset(font_data, pattern) == True) {
	if ((font_data->xlfd_name = get_font_name(oc, pattern)) != NULL) {
	    return True;
	}
    }

    length = strlen (pattern);

    if (length > XLFD_MAX_LEN)
	return -1;

    strcpy(buf, pattern);
    last = buf + length - 1;

    /* Replace the original encoding with the encoding for this FontSet. */

    /* Figure out how many fields have been specified in this xlfd. */
    for (num_fields = 0, base_name = buf; *base_name != '\0'; base_name++)
	if (*base_name == '-') num_fields++;

    switch (num_fields) {
    case 12:
	/* This is the best way to have specified the fontset.  In this
	 * case, there is no original encoding. E.g.,
         *       -*-*-*-*-*-*-14-*-*-*-*-*
	 * To this, we'll append a dash:
         *       -*-*-*-*-*-*-14-*-*-*-*-*-
	 * then append the encoding to get:
         *       -*-*-*-*-*-*-14-*-*-*-*-*-JISX0208.1990-0
	 */
	/*
	 * Take care of:
	 *       -*-*-*-*-*-*-14-*-*-*-*-
	 */
	if (*(last) == '-')
	    *++last = '*';

	*++last = '-';
	break;
    case 13:
	/* Got the charset, not the encoding, zap the charset  In this
	 * case, there is no original encoding, but there is a charset. E.g.,
         *       -*-*-*-*-*-*-14-*-*-*-*-*-jisx0212.1990
	 * To this, we remove the charset:
         *       -*-*-*-*-*-*-14-*-*-*-*-*-
	 * then append the new encoding to get:
         *       -*-*-*-*-*-*-14-*-*-*-*-*-JISX0208.1990-0
	 */
	last = strrchr (buf, '-');
	num_fields = 12;
	break;
    case 14:
	/* Both the charset and the encoding are specified.  Get rid
	 * of them so that we can append the new charset encoding.  E.g.,
         *       -*-*-*-*-*-*-14-*-*-*-*-*-jisx0212.1990-0
	 * To this, we'll remove the encoding and charset to get:
         *       -*-*-*-*-*-*-14-*-*-*-*-*-
	 * then append the new encoding to get:
         *       -*-*-*-*-*-*-14-*-*-*-*-*-JISX0208.1990-0
	 */
	last = strrchr (buf, '-');
	*last = '\0';
	last = strrchr (buf, '-');
	num_fields = 12;
	break;
    default:
	if (*last != '-')
	    *++last = '-';
	break;
    }

   /* At this point, "last" is pointing to the last "-" in the
    * xlfd, and all xlfd's at this point take a form similar to:
    *       -*-*-*-*-*-*-14-*-*-*-*-*-
    * (i.e., no encoding).
    * After the strcpy, we'll end up with something similar to:
    *       -*-*-*-*-*-*-14-*-*-*-*-*-JISX0208.1990-0
    *
    * If the modified font is found in the current FontSet,
    * we've found a match.
    */

    last++;

    if ((last - buf) + strlen(font_data->name) > XLFD_MAX_LEN)
	return -1;

    strcpy(last, font_data->name);
    if ((font_data->xlfd_name = get_font_name(oc, buf)) != NULL)
	return True;

    /* This may not be needed anymore as XListFonts() takes care of this */
    if (num_fields < 12) {
	if ((last - buf) > (XLFD_MAX_LEN - 2))
	    return -1;
	*last = '*';
	*(last + 1) = '-';
	strcpy(last + 2, font_data->name);
	num_fields++;
	last+=2;
	if ((font_data->xlfd_name = get_font_name(oc, buf)) != NULL)
	    return True;
    }


    return False;
}