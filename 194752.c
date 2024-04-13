string_match (const char *string, const char *mask, int case_sensitive)
{
    char last, *mask2;
    int len_string, len_mask, rc;

    if (!mask || !mask[0])
        return 0;

    /* if mask is "*", then any string matches */
    if (strcmp (mask, "*") == 0)
        return 1;

    len_string = strlen (string);
    len_mask = strlen (mask);

    last = mask[len_mask - 1];

    /* mask begins with "*" */
    if ((mask[0] == '*') && (last != '*'))
    {
        /* not enough chars in string to match */
        if (len_string < len_mask - 1)
            return 0;
        /* check if end of string matches */
        if ((case_sensitive && (strcmp (string + len_string - (len_mask - 1),
                                        mask + 1) == 0))
            || (!case_sensitive && (string_strcasecmp (string + len_string - (len_mask - 1),
                                                       mask + 1) == 0)))
            return 1;
        /* no match */
        return 0;
    }

    /* mask ends with "*" */
    if ((mask[0] != '*') && (last == '*'))
    {
        /* not enough chars in string to match */
        if (len_string < len_mask - 1)
            return 0;
        /* check if beginning of string matches */
        if ((case_sensitive && (strncmp (string, mask, len_mask - 1) == 0))
            || (!case_sensitive && (string_strncasecmp (string,
                                                        mask,
                                                        len_mask - 1) == 0)))
            return 1;
        /* no match */
        return 0;
    }

    /* mask begins and ends with "*" */
    if ((mask[0] == '*') && (last == '*'))
    {
        /* not enough chars in string to match */
        if (len_string < len_mask - 1)
            return 0;
        /* keep only relevant chars in mask for searching string */
        mask2 = string_strndup (mask + 1, len_mask - 2);
        if (!mask2)
            return 0;
        /* search string */
        rc = ((case_sensitive && strstr (string, mask2))
              || (!case_sensitive && string_strcasestr (string, mask2))) ?
            1 : 0;
        /* free and return */
        free (mask2);
        return rc;
    }

    /* no "*" at all, compare strings */
    if ((case_sensitive && (strcmp (string, mask) == 0))
        || (!case_sensitive && (string_strcasecmp (string, mask) == 0)))
        return 1;

    /* no match */
    return 0;
}