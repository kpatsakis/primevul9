hook_focus (struct t_weechat_plugin *plugin, const char *area,
            t_hook_callback_focus *callback, void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_focus *new_hook_focus;
    int priority;
    const char *ptr_area;

    if (!area || !area[0] || !callback)
        return NULL;

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
        return NULL;
    new_hook_focus = malloc (sizeof (*new_hook_focus));
    if (!new_hook_focus)
    {
        free (new_hook);
        return NULL;
    }

    hook_get_priority_and_name (area, &priority, &ptr_area);
    hook_init_data (new_hook, plugin, HOOK_TYPE_FOCUS, priority,
                    callback_data);

    new_hook->hook_data = new_hook_focus;
    new_hook_focus->callback = callback;
    new_hook_focus->area = strdup ((ptr_area) ? ptr_area : area);

    hook_add_to_list (new_hook);

    return new_hook;
}