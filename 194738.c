hook_info (struct t_weechat_plugin *plugin, const char *info_name,
           const char *description, const char *args_description,
           t_hook_callback_info *callback, void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_info *new_hook_info;
    int priority;
    const char *ptr_info_name;

    if (!info_name || !info_name[0] || !callback)
        return NULL;

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
        return NULL;
    new_hook_info = malloc (sizeof (*new_hook_info));
    if (!new_hook_info)
    {
        free (new_hook);
        return NULL;
    }

    hook_get_priority_and_name (info_name, &priority, &ptr_info_name);
    hook_init_data (new_hook, plugin, HOOK_TYPE_INFO, priority, callback_data);

    new_hook->hook_data = new_hook_info;
    new_hook_info->callback = callback;
    new_hook_info->info_name = strdup ((ptr_info_name) ?
                                       ptr_info_name : info_name);
    new_hook_info->description = strdup ((description) ? description : "");
    new_hook_info->args_description = strdup ((args_description) ?
                                              args_description : "");

    hook_add_to_list (new_hook);

    return new_hook;
}