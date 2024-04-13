string_strndup (const char *string, int length)
{
    char *result;

    if ((int)strlen (string) < length)
        return strdup (string);

    result = malloc (length + 1);
    if (!result)
        return NULL;

    memcpy (result, string, length);
    result[length] = '\0';

    return result;
}