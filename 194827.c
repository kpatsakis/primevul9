hook_process_child_read (struct t_hook *hook_process, int fd,
                         int index_buffer, struct t_hook **hook_fd)
{
    char buffer[4096];
    int num_read;

    if (hook_process->deleted)
        return;

    num_read = read (fd, buffer, sizeof (buffer) - 1);
    if (num_read > 0)
    {
        hook_process_add_to_buffer (hook_process, index_buffer,
                                    buffer, num_read);
    }
    else if (num_read == 0)
    {
        unhook (*hook_fd);
        *hook_fd = NULL;
    }
}