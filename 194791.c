hook_infolist_get (struct t_weechat_plugin *plugin, const char *infolist_name,
                   void *pointer, const char *arguments)
{
    struct t_hook *ptr_hook, *next_hook;
    struct t_infolist *value;

    /* make C compiler happy */
    (void) plugin;

    if (!infolist_name || !infolist_name[0])
        return NULL;

    hook_exec_start ();

    ptr_hook = weechat_hooks[HOOK_TYPE_INFOLIST];
    while (ptr_hook)
    {
        next_hook = ptr_hook->next_hook;

        if (!ptr_hook->deleted
            && !ptr_hook->running
            && (string_strcasecmp (HOOK_INFOLIST(ptr_hook, infolist_name),
                                   infolist_name) == 0))
        {
            ptr_hook->running = 1;
            value = (HOOK_INFOLIST(ptr_hook, callback))
                (ptr_hook->callback_data, infolist_name, pointer, arguments);
            ptr_hook->running = 0;

            hook_exec_end ();
            return value;
        }

        ptr_hook = next_hook;
    }

    hook_exec_end ();

    /* infolist not found */
    return NULL;
}