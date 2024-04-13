hook_hdata (struct t_weechat_plugin *plugin, const char *hdata_name,
            const char *description,
            t_hook_callback_hdata *callback, void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_hdata *new_hook_hdata;
    int priority;
    const char *ptr_hdata_name;

    if (!hdata_name || !hdata_name[0] || !callback)
        return NULL;

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
        return NULL;
    new_hook_hdata = malloc (sizeof (*new_hook_hdata));
    if (!new_hook_hdata)
    {
        free (new_hook);
        return NULL;
    }

    hook_get_priority_and_name (hdata_name, &priority, &ptr_hdata_name);
    hook_init_data (new_hook, plugin, HOOK_TYPE_HDATA, priority,
                    callback_data);

    new_hook->hook_data = new_hook_hdata;
    new_hook_hdata->callback = callback;
    new_hook_hdata->hdata_name = strdup ((ptr_hdata_name) ?
                                         ptr_hdata_name : hdata_name);
    new_hook_hdata->description = strdup ((description) ? description : "");

    hook_add_to_list (new_hook);

    return new_hook;
}