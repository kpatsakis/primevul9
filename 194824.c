hook_hsignal_send (const char *signal, struct t_hashtable *hashtable)
{
    struct t_hook *ptr_hook, *next_hook;

    hook_exec_start ();

    ptr_hook = weechat_hooks[HOOK_TYPE_HSIGNAL];
    while (ptr_hook)
    {
        next_hook = ptr_hook->next_hook;

        if (!ptr_hook->deleted
            && !ptr_hook->running
            && (string_match (signal, HOOK_HSIGNAL(ptr_hook, signal), 0)))
        {
            ptr_hook->running = 1;
            (void) (HOOK_HSIGNAL(ptr_hook, callback))
                (ptr_hook->callback_data, signal, hashtable);
            ptr_hook->running = 0;
        }

        ptr_hook = next_hook;
    }

    hook_exec_end ();
}