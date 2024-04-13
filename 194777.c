string_strcasestr (const char *string, const char *search)
{
    int length_search;

    length_search = utf8_strlen (search);

    if (!string || !search || (length_search == 0))
        return NULL;

    while (string[0])
    {
        if (string_strncasecmp (string, search, length_search) == 0)
            return (char *)string;

        string = utf8_next_char (string);
    }

    return NULL;
}