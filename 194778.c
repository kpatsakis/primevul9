string_strcasecmp_range (const char *string1, const char *string2, int range)
{
    int diff;

    if (!string1 || !string2)
        return (string1) ? 1 : ((string2) ? -1 : 0);

    while (string1[0] && string2[0])
    {
        diff = utf8_charcasecmp_range (string1, string2, range);
        if (diff != 0)
            return diff;

        string1 = utf8_next_char (string1);
        string2 = utf8_next_char (string2);
    }

    return (string1[0]) ? 1 : ((string2[0]) ? -1 : 0);
}