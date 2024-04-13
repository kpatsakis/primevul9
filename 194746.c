string_replace (const char *string, const char *search, const char *replace)
{
    const char *pos;
    char *new_string;
    int length1, length2, length_new, count;

    if (!string || !search || !replace)
        return NULL;

    length1 = strlen (search);
    length2 = strlen (replace);

    /* count number of strings to replace */
    count = 0;
    pos = string;
    while (pos && pos[0] && (pos = strstr (pos, search)))
    {
        count++;
        pos += length1;
    }

    /* easy: no string to replace! */
    if (count == 0)
        return strdup (string);

    /* compute needed memory for new string */
    length_new = strlen (string) - (count * length1) + (count * length2) + 1;

    /* allocate new string */
    new_string = malloc (length_new);
    if (!new_string)
        return strdup (string);

    /* replace all occurrences */
    new_string[0] = '\0';
    while (string && string[0])
    {
        pos = strstr (string, search);
        if (pos)
        {
            strncat (new_string, string, pos - string);
            strcat (new_string, replace);
            pos += length1;
        }
        else
            strcat (new_string, string);
        string = pos;
    }
    return new_string;
}