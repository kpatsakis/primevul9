hook_info_hashtable (struct t_weechat_plugin *plugin, const char *info_name,
                     const char *description, const char *args_description,
                     const char *output_description,
                     t_hook_callback_info_hashtable *callback,
                     void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_info_hashtable *new_hook_info_hashtable;
    int priority;
    const char *ptr_info_name;

    if (!info_name || !info_name[0] || !callback)
        return NULL;

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
        return NULL;
    new_hook_info_hashtable = malloc (sizeof (*new_hook_info_hashtable));
    if (!new_hook_info_hashtable)
    {
        free (new_hook);
        return NULL;
    }

    hook_get_priority_and_name (info_name, &priority, &ptr_info_name);
    hook_init_data (new_hook, plugin, HOOK_TYPE_INFO_HASHTABLE, priority,
                    callback_data);

    new_hook->hook_data = new_hook_info_hashtable;
    new_hook_info_hashtable->callback = callback;
    new_hook_info_hashtable->info_name = strdup ((ptr_info_name) ?
                                                 ptr_info_name : info_name);
    new_hook_info_hashtable->description = strdup ((description) ? description : "");
    new_hook_info_hashtable->args_description = strdup ((args_description) ?
                                                        args_description : "");
    new_hook_info_hashtable->output_description = strdup ((output_description) ?
                                                          output_description : "");

    hook_add_to_list (new_hook);

    return new_hook;
}