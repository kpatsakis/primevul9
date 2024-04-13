hook_add_to_infolist (struct t_infolist *infolist, const char *arguments)
{
    const char *pos_arguments;
    char *type;
    int i, type_int;

    if (!infolist)
        return 0;

    type = NULL;
    pos_arguments = NULL;

    if (arguments && arguments[0])
    {
        pos_arguments = strchr (arguments, ',');
        if (pos_arguments)
        {
            type = string_strndup (arguments, pos_arguments - arguments);
            pos_arguments++;
        }
        else
            type = strdup (arguments);
    }

    type_int = (type) ? hook_search_type (type) : -1;

    for (i = 0; i < HOOK_NUM_TYPES; i++)
    {
        if ((type_int < 0) || (type_int == i))
            hook_add_to_infolist_type (infolist, i, pos_arguments);
    }

    if (type)
        free (type);

    return 1;
}