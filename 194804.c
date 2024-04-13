hook_hsignal (struct t_weechat_plugin *plugin, const char *signal,
              t_hook_callback_hsignal *callback, void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_hsignal *new_hook_hsignal;
    int priority;
    const char *ptr_signal;

    if (!signal || !signal[0] || !callback)
        return NULL;

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
        return NULL;
    new_hook_hsignal = malloc (sizeof (*new_hook_hsignal));
    if (!new_hook_hsignal)
    {
        free (new_hook);
        return NULL;
    }

    hook_get_priority_and_name (signal, &priority, &ptr_signal);
    hook_init_data (new_hook, plugin, HOOK_TYPE_HSIGNAL, priority,
                    callback_data);

    new_hook->hook_data = new_hook_hsignal;
    new_hook_hsignal->callback = callback;
    new_hook_hsignal->signal = strdup ((ptr_signal) ? ptr_signal : signal);

    hook_add_to_list (new_hook);

    return new_hook;
}