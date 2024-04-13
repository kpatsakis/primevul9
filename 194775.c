hook_get_priority_and_name (const char *name,
                            int *priority, const char **ptr_name)
{
    char *pos, *str_priority, *error;
    long number;

    if (priority)
        *priority = HOOK_PRIORITY_DEFAULT;
    if (ptr_name)
        *ptr_name = name;

    pos = strchr (name, '|');
    if (pos)
    {
        str_priority = string_strndup (name, pos - name);
        if (str_priority)
        {
            error = NULL;
            number = strtol (str_priority, &error, 10);
            if (error && !error[0])
            {
                if (priority)
                    *priority = number;
                if (ptr_name)
                    *ptr_name = pos + 1;
            }
            free (str_priority);
        }
    }
}