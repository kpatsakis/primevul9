hook_process_hashtable (struct t_weechat_plugin *plugin,
                        const char *command, struct t_hashtable *options,
                        int timeout,
                        t_hook_callback_process *callback, void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_process *new_hook_process;
    char *stdout_buffer, *stderr_buffer;

    if (!command || !command[0] || !callback)
        return NULL;

    stdout_buffer = malloc (HOOK_PROCESS_BUFFER_SIZE + 1);
    if (!stdout_buffer)
        return NULL;

    stderr_buffer = malloc (HOOK_PROCESS_BUFFER_SIZE + 1);
    if (!stderr_buffer)
    {
        free (stdout_buffer);
        return NULL;
    }

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
    {
        free (stdout_buffer);
        free (stderr_buffer);
        return NULL;
    }
    new_hook_process = malloc (sizeof (*new_hook_process));
    if (!new_hook_process)
    {
        free (stdout_buffer);
        free (stderr_buffer);
        free (new_hook);
        return NULL;
    }

    hook_init_data (new_hook, plugin, HOOK_TYPE_PROCESS, HOOK_PRIORITY_DEFAULT,
                    callback_data);

    new_hook->hook_data = new_hook_process;
    new_hook_process->callback = callback;
    new_hook_process->command = strdup (command);
    new_hook_process->options = (options) ? hashtable_dup (options) : NULL;
    new_hook_process->timeout = timeout;
    new_hook_process->child_read[HOOK_PROCESS_STDOUT] = -1;
    new_hook_process->child_read[HOOK_PROCESS_STDERR] = -1;
    new_hook_process->child_write[HOOK_PROCESS_STDOUT] = -1;
    new_hook_process->child_write[HOOK_PROCESS_STDERR] = -1;
    new_hook_process->child_pid = 0;
    new_hook_process->hook_fd[HOOK_PROCESS_STDOUT] = NULL;
    new_hook_process->hook_fd[HOOK_PROCESS_STDERR] = NULL;
    new_hook_process->hook_timer = NULL;
    new_hook_process->buffer[HOOK_PROCESS_STDOUT] = stdout_buffer;
    new_hook_process->buffer[HOOK_PROCESS_STDERR] = stderr_buffer;
    new_hook_process->buffer_size[HOOK_PROCESS_STDOUT] = 0;
    new_hook_process->buffer_size[HOOK_PROCESS_STDERR] = 0;

    hook_add_to_list (new_hook);

    hook_process_run (new_hook);

    return new_hook;
}