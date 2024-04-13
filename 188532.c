static void execute_fsfreeze_hook(FsfreezeHookArg arg, Error **errp)
{
    int status;
    pid_t pid;
    const char *hook;
    const char *arg_str = fsfreeze_hook_arg_string[arg];
    Error *local_err = NULL;

    hook = ga_fsfreeze_hook(ga_state);
    if (!hook) {
        return;
    }
    if (access(hook, X_OK) != 0) {
        error_setg_errno(errp, errno, "can't access fsfreeze hook '%s'", hook);
        return;
    }

    slog("executing fsfreeze hook with arg '%s'", arg_str);
    pid = fork();
    if (pid == 0) {
        setsid();
        reopen_fd_to_null(0);
        reopen_fd_to_null(1);
        reopen_fd_to_null(2);

        execle(hook, hook, arg_str, NULL, environ);
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        error_setg_errno(errp, errno, "failed to create child process");
        return;
    }

    ga_wait_child(pid, &status, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return;
    }

    if (!WIFEXITED(status)) {
        error_setg(errp, "fsfreeze hook has terminated abnormally");
        return;
    }

    status = WEXITSTATUS(status);
    if (status) {
        error_setg(errp, "fsfreeze hook has failed with status %d", status);
        return;
    }
}