hook_modifier_exec (struct t_weechat_plugin *plugin, const char *modifier,
                    const char *modifier_data, const char *string)
{
    struct t_hook *ptr_hook, *next_hook;
    char *new_msg, *message_modified;

    /* make C compiler happy */
    (void) plugin;

    if (!modifier || !modifier[0])
        return NULL;

    new_msg = NULL;
    message_modified = strdup (string);
    if (!message_modified)
        return NULL;

    hook_exec_start ();

    ptr_hook = weechat_hooks[HOOK_TYPE_MODIFIER];
    while (ptr_hook)
    {
        next_hook = ptr_hook->next_hook;

        if (!ptr_hook->deleted
            && !ptr_hook->running
            && (string_strcasecmp (HOOK_MODIFIER(ptr_hook, modifier),
                                   modifier) == 0))
        {
            ptr_hook->running = 1;
            new_msg = (HOOK_MODIFIER(ptr_hook, callback))
                (ptr_hook->callback_data, modifier, modifier_data,
                 message_modified);
            ptr_hook->running = 0;

            /* empty string returned => message dropped */
            if (new_msg && !new_msg[0])
            {
                free (message_modified);
                hook_exec_end ();
                return new_msg;
            }

            /* new message => keep it as base for next modifier */
            if (new_msg)
            {
                free (message_modified);
                message_modified = new_msg;
            }
        }

        ptr_hook = next_hook;
    }

    hook_exec_end ();

    return message_modified;
}