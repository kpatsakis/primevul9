hook_find_pos (struct t_hook *hook)
{
    struct t_hook *ptr_hook;
    int rc_cmp;

    if (hook->type == HOOK_TYPE_COMMAND)
    {
        /* for command hook, sort on command name */
        for (ptr_hook = weechat_hooks[hook->type]; ptr_hook;
             ptr_hook = ptr_hook->next_hook)
        {
            if (!ptr_hook->deleted)
            {
                rc_cmp = string_strcasecmp (HOOK_COMMAND(hook, command),
                                            HOOK_COMMAND(ptr_hook, command));
                if (rc_cmp < 0)
                    return ptr_hook;
                if ((rc_cmp == 0) && (hook->priority > ptr_hook->priority))
                    return ptr_hook;
            }
        }
    }
    else
    {
        /* for other types, sort on priority */
        for (ptr_hook = weechat_hooks[hook->type]; ptr_hook;
             ptr_hook = ptr_hook->next_hook)
        {
            if (!ptr_hook->deleted && (hook->priority > ptr_hook->priority))
                return ptr_hook;
        }
    }

    /* position not found, add at the end */
    return NULL;
}