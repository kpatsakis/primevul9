hook_config (struct t_weechat_plugin *plugin, const char *option,
             t_hook_callback_config *callback, void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_config *new_hook_config;
    int priority;
    const char *ptr_option;

    if (!callback)
        return NULL;

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
        return NULL;
    new_hook_config = malloc (sizeof (*new_hook_config));
    if (!new_hook_config)
    {
        free (new_hook);
        return NULL;
    }

    hook_get_priority_and_name (option, &priority, &ptr_option);
    hook_init_data (new_hook, plugin, HOOK_TYPE_CONFIG, priority,
                    callback_data);

    new_hook->hook_data = new_hook_config;
    new_hook_config->callback = callback;
    new_hook_config->option = strdup ((ptr_option) ? ptr_option :
                                      ((option) ? option : ""));

    hook_add_to_list (new_hook);

    return new_hook;
}