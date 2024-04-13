hook_search_command (struct t_weechat_plugin *plugin, const char *command)
{
    struct t_hook *ptr_hook;

    for (ptr_hook = weechat_hooks[HOOK_TYPE_COMMAND]; ptr_hook;
         ptr_hook = ptr_hook->next_hook)
    {
        if (!ptr_hook->deleted
            && (ptr_hook->plugin == plugin)
            && (string_strcasecmp (HOOK_COMMAND(ptr_hook, command), command) == 0))
            return ptr_hook;
    }

    /* command hook not found for plugin */
    return NULL;
}