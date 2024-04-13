hook_remove_deleted ()
{
    int type;
    struct t_hook *ptr_hook, *next_hook;

    if (real_delete_pending)
    {
        for (type = 0; type < HOOK_NUM_TYPES; type++)
        {
            ptr_hook = weechat_hooks[type];
            while (ptr_hook)
            {
                next_hook = ptr_hook->next_hook;

                if (ptr_hook->deleted)
                    hook_remove_from_list (ptr_hook);

                ptr_hook = next_hook;
            }
        }
        real_delete_pending = 0;
    }
}