static void bios_supports_mode(const char *pmutils_bin, const char *pmutils_arg,
                               const char *sysfile_str, Error **errp)
{
    Error *local_err = NULL;
    char *pmutils_path;
    pid_t pid;
    int status;

    pmutils_path = g_find_program_in_path(pmutils_bin);

    pid = fork();
    if (!pid) {
        char buf[32]; /* hopefully big enough */
        ssize_t ret;
        int fd;

        setsid();
        reopen_fd_to_null(0);
        reopen_fd_to_null(1);
        reopen_fd_to_null(2);

        if (pmutils_path) {
            execle(pmutils_path, pmutils_bin, pmutils_arg, NULL, environ);
        }

        /*
         * If we get here either pm-utils is not installed or execle() has
         * failed. Let's try the manual method if the caller wants it.
         */

        if (!sysfile_str) {
            _exit(SUSPEND_NOT_SUPPORTED);
        }

        fd = open(LINUX_SYS_STATE_FILE, O_RDONLY);
        if (fd < 0) {
            _exit(SUSPEND_NOT_SUPPORTED);
        }

        ret = read(fd, buf, sizeof(buf)-1);
        if (ret <= 0) {
            _exit(SUSPEND_NOT_SUPPORTED);
        }
        buf[ret] = '\0';

        if (strstr(buf, sysfile_str)) {
            _exit(SUSPEND_SUPPORTED);
        }

        _exit(SUSPEND_NOT_SUPPORTED);
    } else if (pid < 0) {
        error_setg_errno(errp, errno, "failed to create child process");
        goto out;
    }

    ga_wait_child(pid, &status, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        goto out;
    }

    if (!WIFEXITED(status)) {
        error_setg(errp, "child process has terminated abnormally");
        goto out;
    }

    switch (WEXITSTATUS(status)) {
    case SUSPEND_SUPPORTED:
        goto out;
    case SUSPEND_NOT_SUPPORTED:
        error_setg(errp,
                   "the requested suspend mode is not supported by the guest");
        goto out;
    default:
        error_setg(errp,
                   "the helper program '%s' returned an unexpected exit status"
                   " code (%d)", pmutils_path, WEXITSTATUS(status));
        goto out;
    }

out:
    g_free(pmutils_path);
}