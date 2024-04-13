hook_completion_exec (struct t_weechat_plugin *plugin,
                      const char *completion_item,
                      struct t_gui_buffer *buffer,
                      struct t_gui_completion *completion)
{
    struct t_hook *ptr_hook, *next_hook;

    /* make C compiler happy */
    (void) plugin;

    hook_exec_start ();

    ptr_hook = weechat_hooks[HOOK_TYPE_COMPLETION];
    while (ptr_hook)
    {
        next_hook = ptr_hook->next_hook;

        if (!ptr_hook->deleted
            && !ptr_hook->running
            && (string_strcasecmp (HOOK_COMPLETION(ptr_hook, completion_item),
                                   completion_item) == 0))
        {
            ptr_hook->running = 1;
            (void) (HOOK_COMPLETION(ptr_hook, callback))
                (ptr_hook->callback_data, completion_item, buffer, completion);
            ptr_hook->running = 0;
        }

        ptr_hook = next_hook;
    }

    hook_exec_end ();
}