string_build_with_split_string (const char **split_string,
                                const char *separator)
{
    int i, length, length_separator;
    char *result;

    if (!split_string)
        return NULL;

    length = 0;
    length_separator = (separator) ? strlen (separator) : 0;

    for (i = 0; split_string[i]; i++)
    {
        length += strlen (split_string[i]) + length_separator;
    }

    result = malloc (length + 1);
    if (result)
    {
        result[0] = '\0';

        for (i = 0; split_string[i]; i++)
        {
            strcat (result, split_string[i]);
            if (separator && split_string[i + 1])
                strcat (result, separator);
        }
    }

    return result;
}