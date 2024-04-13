hook_fd_exec (fd_set *read_fds, fd_set *write_fds, fd_set *exception_fds)
{
    struct t_hook *ptr_hook, *next_hook;

    hook_exec_start ();

    ptr_hook = weechat_hooks[HOOK_TYPE_FD];
    while (ptr_hook)
    {
        next_hook = ptr_hook->next_hook;

        if (!ptr_hook->deleted
            && !ptr_hook->running
            && (((HOOK_FD(ptr_hook, flags) & HOOK_FD_FLAG_READ)
                 && (FD_ISSET(HOOK_FD(ptr_hook, fd), read_fds)))
                || ((HOOK_FD(ptr_hook, flags) & HOOK_FD_FLAG_WRITE)
                    && (FD_ISSET(HOOK_FD(ptr_hook, fd), write_fds)))
                || ((HOOK_FD(ptr_hook, flags) & HOOK_FD_FLAG_EXCEPTION)
                    && (FD_ISSET(HOOK_FD(ptr_hook, fd), exception_fds)))))
        {
            ptr_hook->running = 1;
            (void) (HOOK_FD(ptr_hook, callback)) (ptr_hook->callback_data,
                                                  HOOK_FD(ptr_hook, fd));
            ptr_hook->running = 0;
        }

        ptr_hook = next_hook;
    }

    hook_exec_end ();
}