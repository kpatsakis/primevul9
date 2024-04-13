hook_completion (struct t_weechat_plugin *plugin, const char *completion_item,
                 const char *description,
                 t_hook_callback_completion *callback, void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_completion *new_hook_completion;
    int priority;
    const char *ptr_completion_item;

    if (!completion_item || !completion_item[0]
        || strchr (completion_item, ' ') || !callback)
        return NULL;

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
        return NULL;
    new_hook_completion = malloc (sizeof (*new_hook_completion));
    if (!new_hook_completion)
    {
        free (new_hook);
        return NULL;
    }

    hook_get_priority_and_name (completion_item, &priority, &ptr_completion_item);
    hook_init_data (new_hook, plugin, HOOK_TYPE_COMPLETION, priority,
                    callback_data);

    new_hook->hook_data = new_hook_completion;
    new_hook_completion->callback = callback;
    new_hook_completion->completion_item = strdup ((ptr_completion_item) ?
                                                   ptr_completion_item : completion_item);
    new_hook_completion->description = strdup ((description) ?
                                               description : "");

    hook_add_to_list (new_hook);

    return new_hook;
}