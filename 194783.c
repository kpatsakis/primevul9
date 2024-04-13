hook_search_fd (int fd)
{
    struct t_hook *ptr_hook;

    for (ptr_hook = weechat_hooks[HOOK_TYPE_FD]; ptr_hook;
         ptr_hook = ptr_hook->next_hook)
    {
        if (!ptr_hook->deleted && (HOOK_FD(ptr_hook, fd) == fd))
            return ptr_hook;
    }

    /* fd hook not found */
    return NULL;
}