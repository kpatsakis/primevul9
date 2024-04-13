hook_remove_from_list (struct t_hook *hook)
{
    struct t_hook *new_hooks;
    int type;

    type = hook->type;

    if (last_weechat_hook[hook->type] == hook)
        last_weechat_hook[hook->type] = hook->prev_hook;
    if (hook->prev_hook)
    {
        (hook->prev_hook)->next_hook = hook->next_hook;
        new_hooks = weechat_hooks[hook->type];
    }
    else
        new_hooks = hook->next_hook;

    if (hook->next_hook)
        (hook->next_hook)->prev_hook = hook->prev_hook;

    free (hook);

    weechat_hooks[type] = new_hooks;
}