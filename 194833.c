hook_fd (struct t_weechat_plugin *plugin, int fd, int flag_read,
         int flag_write, int flag_exception,
         t_hook_callback_fd *callback, void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_fd *new_hook_fd;

    if ((fd < 0) || hook_search_fd (fd) || !callback)
        return NULL;

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
        return NULL;
    new_hook_fd = malloc (sizeof (*new_hook_fd));
    if (!new_hook_fd)
    {
        free (new_hook);
        return NULL;
    }

    hook_init_data (new_hook, plugin, HOOK_TYPE_FD, HOOK_PRIORITY_DEFAULT,
                    callback_data);

    new_hook->hook_data = new_hook_fd;
    new_hook_fd->callback = callback;
    new_hook_fd->fd = fd;
    new_hook_fd->flags = 0;
    new_hook_fd->error = 0;
    if (flag_read)
        new_hook_fd->flags |= HOOK_FD_FLAG_READ;
    if (flag_write)
        new_hook_fd->flags |= HOOK_FD_FLAG_WRITE;
    if (flag_exception)
        new_hook_fd->flags |= HOOK_FD_FLAG_EXCEPTION;

    hook_add_to_list (new_hook);

    return new_hook;
}