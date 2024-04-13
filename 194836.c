string_strncasecmp_range (const char *string1, const char *string2, int max,
                          int range)
{
    int count, diff;

    if (!string1 || !string2)
        return (string1) ? 1 : ((string2) ? -1 : 0);

    count = 0;
    while ((count < max) && string1[0] && string2[0])
    {
        diff = utf8_charcasecmp_range (string1, string2, range);
        if (diff != 0)
            return diff;

        string1 = utf8_next_char (string1);
        string2 = utf8_next_char (string2);
        count++;
    }

    if (count >= max)
        return 0;
    else
        return (string1[0]) ? 1 : ((string2[0]) ? -1 : 0);
}