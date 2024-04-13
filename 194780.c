string_strcmp_ignore_chars (const char *string1, const char *string2,
                            const char *chars_ignored, int case_sensitive)
{
    int diff;

    if (!string1 && !string2)
        return 0;
    if (!string1 && string2)
        return -1;
    if (string1 && !string2)
        return 1;

    while (string1 && string1[0] && string2 && string2[0])
    {
        /* skip ignored chars */
        while (string1 && string1[0] && strchr (chars_ignored, string1[0]))
        {
            string1 = utf8_next_char (string1);
        }
        while (string2 && string2[0] && strchr (chars_ignored, string2[0]))
        {
            string2 = utf8_next_char (string2);
        }

        /* end of one (or both) string(s) ? */
        if ((!string1 || !string1[0]) && (!string2 || !string2[0]))
            return 0;
        if ((!string1 || !string1[0]) && string2 && string2[0])
            return -1;
        if (string1 && string1[0] && (!string2 || !string2[0]))
            return 1;

        /* look at diff */
        diff = (case_sensitive) ?
            (int)string1[0] - (int)string2[0] : utf8_charcasecmp (string1, string2);
        if (diff != 0)
            return diff;

        string1 = utf8_next_char (string1);
        string2 = utf8_next_char (string2);

        /* skip ignored chars */
        while (string1 && string1[0] && strchr (chars_ignored, string1[0]))
        {
            string1 = utf8_next_char (string1);
        }
        while (string2 && string2[0] && strchr (chars_ignored, string2[0]))
        {
            string2 = utf8_next_char (string2);
        }
    }
    if ((!string1 || !string1[0]) && string2 && string2[0])
        return -1;
    if (string1 && string1[0] && (!string2 || !string2[0]))
        return 1;
    return 0;
}