hook_process_child_read_stderr_cb (void *arg_hook_process, int fd)
{
    struct t_hook *hook_process;

    hook_process = (struct t_hook *)arg_hook_process;
    hook_process_child_read (hook_process, fd, HOOK_PROCESS_STDERR,
                             &(HOOK_PROCESS(hook_process, hook_fd[HOOK_PROCESS_STDERR])));
    return WEECHAT_RC_OK;
}