string_expand_home (const char *path)
{
    char *ptr_home, *str;
    int length;

    if (!path)
        return NULL;

    if (!path[0] || (path[0] != '~')
        || ((path[1] && path[1] != DIR_SEPARATOR_CHAR)))
    {
        return strdup (path);
    }

    ptr_home = getenv ("HOME");

    length = strlen (ptr_home) + strlen (path + 1) + 1;
    str = malloc (length);
    if (!str)
        return strdup (path);

    snprintf (str, length, "%s%s", ptr_home, path + 1);

    return str;
}