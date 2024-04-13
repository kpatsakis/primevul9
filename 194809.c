string_split_command (const char *command, char separator)
{
    int nb_substr, arr_idx, str_idx, type;
    char **array, **array2;
    char *buffer, *p;
    const char *ptr;

    if (!command || !command[0])
        return NULL;

    nb_substr = 1;
    ptr = command;
    while ( (p = strchr(ptr, separator)) != NULL)
    {
        nb_substr++;
        ptr = ++p;
    }

    array = malloc ((nb_substr + 1) * sizeof (array[0]));
    if (!array)
        return NULL;

    buffer = malloc (strlen(command) + 1);
    if (!buffer)
    {
        free (array);
        return NULL;
    }

    ptr = command;
    str_idx = 0;
    arr_idx = 0;
    while(*ptr != '\0')
    {
        type = 0;
        if (*ptr == ';')
        {
            if (ptr == command)
                type = 1;
            else if ( *(ptr-1) != '\\')
                type = 1;
            else if ( *(ptr-1) == '\\')
                type = 2;
        }
        if (type == 1)
        {
            buffer[str_idx] = '\0';
            str_idx = -1;
            p = buffer;
            /* strip white spaces a the begining of the line */
            while (*p == ' ') p++;
            if (p  && p[0])
                array[arr_idx++] = strdup (p);
        }
        else if (type == 2)
            buffer[--str_idx] = *ptr;
        else
            buffer[str_idx] = *ptr;
        str_idx++;
        ptr++;
    }

    buffer[str_idx] = '\0';
    p = buffer;
    while (*p == ' ') p++;
    if (p  && p[0])
        array[arr_idx++] = strdup (p);

    array[arr_idx] = NULL;

    free (buffer);

    array2 = realloc (array, (arr_idx + 1) * sizeof(array[0]));
    if (!array2)
    {
        if (array)
            free (array);
        return NULL;
    }

    return array2;
}