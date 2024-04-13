hook_signal_send (const char *signal, const char *type_data, void *signal_data)
{
    struct t_hook *ptr_hook, *next_hook;

    hook_exec_start ();

    ptr_hook = weechat_hooks[HOOK_TYPE_SIGNAL];
    while (ptr_hook)
    {
        next_hook = ptr_hook->next_hook;

        if (!ptr_hook->deleted
            && !ptr_hook->running
            && (string_match (signal, HOOK_SIGNAL(ptr_hook, signal), 0)))
        {
            ptr_hook->running = 1;
            (void) (HOOK_SIGNAL(ptr_hook, callback))
                (ptr_hook->callback_data, signal, type_data, signal_data);
            ptr_hook->running = 0;
        }

        ptr_hook = next_hook;
    }

    hook_exec_end ();
}