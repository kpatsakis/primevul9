hook_infolist (struct t_weechat_plugin *plugin, const char *infolist_name,
               const char *description, const char *pointer_description,
               const char *args_description,
               t_hook_callback_infolist *callback, void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_infolist *new_hook_infolist;
    int priority;
    const char *ptr_infolist_name;

    if (!infolist_name || !infolist_name[0] || !callback)
        return NULL;

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
        return NULL;
    new_hook_infolist = malloc (sizeof (*new_hook_infolist));
    if (!new_hook_infolist)
    {
        free (new_hook);
        return NULL;
    }

    hook_get_priority_and_name (infolist_name, &priority, &ptr_infolist_name);
    hook_init_data (new_hook, plugin, HOOK_TYPE_INFOLIST, priority,
                    callback_data);

    new_hook->hook_data = new_hook_infolist;
    new_hook_infolist->callback = callback;
    new_hook_infolist->infolist_name = strdup ((ptr_infolist_name) ?
                                               ptr_infolist_name : infolist_name);
    new_hook_infolist->description = strdup ((description) ? description : "");
    new_hook_infolist->pointer_description = strdup ((pointer_description) ?
                                                     pointer_description : "");
    new_hook_infolist->args_description = strdup ((args_description) ?
                                                  args_description : "");

    hook_add_to_list (new_hook);

    return new_hook;
}