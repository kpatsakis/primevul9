hook_set (struct t_hook *hook, const char *property, const char *value)
{
    if (string_strcasecmp (property, "subplugin") == 0)
    {
        if (hook->subplugin)
            free(hook->subplugin);
        hook->subplugin = strdup (value);
    }
}