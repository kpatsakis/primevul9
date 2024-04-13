hook_init ()
{
    int type;

    for (type = 0; type < HOOK_NUM_TYPES; type++)
    {
        weechat_hooks[type] = NULL;
        last_weechat_hook[type] = NULL;
    }
    hook_last_system_time = time (NULL);
}