hook_fd_set (fd_set *read_fds, fd_set *write_fds, fd_set *exception_fds)
{
    struct t_hook *ptr_hook;
    int max_fd;

    max_fd = 0;
    for (ptr_hook = weechat_hooks[HOOK_TYPE_FD]; ptr_hook;
         ptr_hook = ptr_hook->next_hook)
    {
        if (!ptr_hook->deleted)
        {
            /* skip invalid file descriptors */
            if ((fcntl (HOOK_FD(ptr_hook,fd), F_GETFD) == -1)
                && (errno == EBADF))
            {
                if (HOOK_FD(ptr_hook, error) == 0)
                {
                    HOOK_FD(ptr_hook, error) = errno;
                    gui_chat_printf (NULL,
                                     _("%sError: bad file descriptor (%d) "
                                       "used in hook_fd"),
                                     gui_chat_prefix[GUI_CHAT_PREFIX_ERROR],
                                     HOOK_FD(ptr_hook, fd));
                }
            }
            else
            {
                if (HOOK_FD(ptr_hook, flags) & HOOK_FD_FLAG_READ)
                {
                    FD_SET (HOOK_FD(ptr_hook, fd), read_fds);
                    if (HOOK_FD(ptr_hook, fd) > max_fd)
                        max_fd = HOOK_FD(ptr_hook, fd);
                }
                if (HOOK_FD(ptr_hook, flags) & HOOK_FD_FLAG_WRITE)
                {
                    FD_SET (HOOK_FD(ptr_hook, fd), write_fds);
                    if (HOOK_FD(ptr_hook, fd) > max_fd)
                        max_fd = HOOK_FD(ptr_hook, fd);
                }
                if (HOOK_FD(ptr_hook, flags) & HOOK_FD_FLAG_EXCEPTION)
                {
                    FD_SET (HOOK_FD(ptr_hook, fd), exception_fds);
                    if (HOOK_FD(ptr_hook, fd) > max_fd)
                        max_fd = HOOK_FD(ptr_hook, fd);
                }
            }
        }
    }

    return max_fd;
}