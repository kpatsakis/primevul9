unhook_all_plugin (struct t_weechat_plugin *plugin)
{
    int type;
    struct t_hook *ptr_hook, *next_hook;

    for (type = 0; type < HOOK_NUM_TYPES; type++)
    {
        ptr_hook = weechat_hooks[type];
        while (ptr_hook)
        {
            next_hook = ptr_hook->next_hook;
            if (ptr_hook->plugin == plugin)
                unhook (ptr_hook);
            ptr_hook = next_hook;
        }
    }
}