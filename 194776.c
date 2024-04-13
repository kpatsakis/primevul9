hook_info_get_hashtable (struct t_weechat_plugin *plugin, const char *info_name,
                         struct t_hashtable *hashtable)
{
    struct t_hook *ptr_hook, *next_hook;
    struct t_hashtable *value;

    /* make C compiler happy */
    (void) plugin;

    if (!info_name || !info_name[0])
        return NULL;

    hook_exec_start ();

    ptr_hook = weechat_hooks[HOOK_TYPE_INFO_HASHTABLE];
    while (ptr_hook)
    {
        next_hook = ptr_hook->next_hook;

        if (!ptr_hook->deleted
            && !ptr_hook->running
            && (string_strcasecmp (HOOK_INFO_HASHTABLE(ptr_hook, info_name),
                                   info_name) == 0))
        {
            ptr_hook->running = 1;
            value = (HOOK_INFO_HASHTABLE(ptr_hook, callback))
                (ptr_hook->callback_data, info_name, hashtable);
            ptr_hook->running = 0;

            hook_exec_end ();
            return value;
        }

        ptr_hook = next_hook;
    }

    hook_exec_end ();

    /* info not found */
    return NULL;
}