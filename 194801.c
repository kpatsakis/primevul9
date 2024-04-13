hook_modifier (struct t_weechat_plugin *plugin, const char *modifier,
               t_hook_callback_modifier *callback, void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_modifier *new_hook_modifier;
    int priority;
    const char *ptr_modifier;

    if (!modifier || !modifier[0] || !callback)
        return NULL;

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
        return NULL;
    new_hook_modifier = malloc (sizeof (*new_hook_modifier));
    if (!new_hook_modifier)
    {
        free (new_hook);
        return NULL;
    }

    hook_get_priority_and_name (modifier, &priority, &ptr_modifier);
    hook_init_data (new_hook, plugin, HOOK_TYPE_MODIFIER, priority,
                    callback_data);

    new_hook->hook_data = new_hook_modifier;
    new_hook_modifier->callback = callback;
    new_hook_modifier->modifier = strdup ((ptr_modifier) ? ptr_modifier : modifier);

    hook_add_to_list (new_hook);

    return new_hook;
}