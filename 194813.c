hook_timer (struct t_weechat_plugin *plugin, long interval, int align_second,
            int max_calls, t_hook_callback_timer *callback,
            void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_timer *new_hook_timer;

    if ((interval <= 0) || !callback)
        return NULL;

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
        return NULL;
    new_hook_timer = malloc (sizeof (*new_hook_timer));
    if (!new_hook_timer)
    {
        free (new_hook);
        return NULL;
    }

    hook_init_data (new_hook, plugin, HOOK_TYPE_TIMER, HOOK_PRIORITY_DEFAULT,
                    callback_data);

    new_hook->hook_data = new_hook_timer;
    new_hook_timer->callback = callback;
    new_hook_timer->interval = interval;
    new_hook_timer->align_second = align_second;
    new_hook_timer->remaining_calls = max_calls;

    hook_timer_init (new_hook);

    hook_add_to_list (new_hook);

    return new_hook;
}