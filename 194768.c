hook_process_timer_cb (void *arg_hook_process, int remaining_calls)
{
    struct t_hook *hook_process;
    int status, rc;

    /* make C compiler happy */
    (void) remaining_calls;

    hook_process = (struct t_hook *)arg_hook_process;

    if (hook_process->deleted)
        return WEECHAT_RC_OK;

    if (remaining_calls == 0)
    {
        hook_process_send_buffers (hook_process, WEECHAT_HOOK_PROCESS_ERROR);
        if (weechat_debug_core >= 1)
        {
            gui_chat_printf (NULL,
                             _("End of command '%s', timeout reached (%.1fs)"),
                             HOOK_PROCESS(hook_process, command),
                             ((float)HOOK_PROCESS(hook_process, timeout)) / 1000);
        }
        kill (HOOK_PROCESS(hook_process, child_pid), SIGKILL);
        usleep (1000);
        unhook (hook_process);
    }
    else
    {
        if (!HOOK_PROCESS(hook_process, hook_fd[HOOK_PROCESS_STDOUT])
            && !HOOK_PROCESS(hook_process, hook_fd[HOOK_PROCESS_STDERR]))
        {
            if (waitpid (HOOK_PROCESS(hook_process, child_pid), &status, WNOHANG) > 0)
            {
                rc = WEXITSTATUS(status);
                hook_process_send_buffers (hook_process, rc);
                unhook (hook_process);
            }
        }
    }

    return WEECHAT_RC_OK;
}