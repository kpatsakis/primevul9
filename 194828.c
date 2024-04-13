hook_search_type (const char *type)
{
    int i;

    if (!type)
        return -1;

    for (i = 0; i < HOOK_NUM_TYPES; i++)
    {
        if (strcmp (hook_type_string[i], type) == 0)
            return i;
    }

    /* type not found */
    return -1;
}