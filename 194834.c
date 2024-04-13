string_strcasecmp (const char *string1, const char *string2)
{
    int diff;

    if (!string1 || !string2)
        return (string1) ? 1 : ((string2) ? -1 : 0);

    while (string1[0] && string2[0])
    {
        diff = utf8_charcasecmp (string1, string2);
        if (diff != 0)
            return diff;

        string1 = utf8_next_char (string1);
        string2 = utf8_next_char (string2);
    }

    return (string1[0]) ? 1 : ((string2[0]) ? -1 : 0);
}