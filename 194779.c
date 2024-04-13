hook_process_send_buffers (struct t_hook *hook_process, int callback_rc)
{
    int i, size;

    /* add '\0' at end of stdout and stderr */
    for (i = 0; i < 2; i++)
    {
        size = HOOK_PROCESS(hook_process, buffer_size[i]);
        if (size > 0)
            HOOK_PROCESS(hook_process, buffer[i])[size] = '\0';
    }

    /* send buffers to callback */
    (void) (HOOK_PROCESS(hook_process, callback))
        (hook_process->callback_data,
         HOOK_PROCESS(hook_process, command),
         callback_rc,
         (HOOK_PROCESS(hook_process, buffer_size[HOOK_PROCESS_STDOUT]) > 0) ?
         HOOK_PROCESS(hook_process, buffer[HOOK_PROCESS_STDOUT]) : NULL,
         (HOOK_PROCESS(hook_process, buffer_size[HOOK_PROCESS_STDERR]) > 0) ?
         HOOK_PROCESS(hook_process, buffer[HOOK_PROCESS_STDERR]) : NULL);

    /* reset size for stdout and stderr */
    HOOK_PROCESS(hook_process, buffer_size[HOOK_PROCESS_STDOUT]) = 0;
    HOOK_PROCESS(hook_process, buffer_size[HOOK_PROCESS_STDERR]) = 0;
}