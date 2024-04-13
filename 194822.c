hook_config_exec (const char *option, const char *value)
{
    struct t_hook *ptr_hook, *next_hook;

    hook_exec_start ();

    ptr_hook = weechat_hooks[HOOK_TYPE_CONFIG];
    while (ptr_hook)
    {
        next_hook = ptr_hook->next_hook;

        if (!ptr_hook->deleted
            && !ptr_hook->running
            && (!HOOK_CONFIG(ptr_hook, option)
                || (string_match (option, HOOK_CONFIG(ptr_hook, option), 0))))
        {
            ptr_hook->running = 1;
            (void) (HOOK_CONFIG(ptr_hook, callback))
                (ptr_hook->callback_data, option, value);
            ptr_hook->running = 0;
        }

        ptr_hook = next_hook;
    }

    hook_exec_end ();
}