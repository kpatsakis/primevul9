static void guest_suspend(const char *pmutils_bin, const char *sysfile_str,
                          Error **errp)
{
    Error *local_err = NULL;
    char *pmutils_path;
    pid_t pid;
    int status;

    pmutils_path = g_find_program_in_path(pmutils_bin);

    pid = fork();
    if (pid == 0) {
        /* child */
        int fd;

        setsid();
        reopen_fd_to_null(0);
        reopen_fd_to_null(1);
        reopen_fd_to_null(2);

        if (pmutils_path) {
            execle(pmutils_path, pmutils_bin, NULL, environ);
        }

        /*
         * If we get here either pm-utils is not installed or execle() has
         * failed. Let's try the manual method if the caller wants it.
         */

        if (!sysfile_str) {
            _exit(EXIT_FAILURE);
        }

        fd = open(LINUX_SYS_STATE_FILE, O_WRONLY);
        if (fd < 0) {
            _exit(EXIT_FAILURE);
        }

        if (write(fd, sysfile_str, strlen(sysfile_str)) < 0) {
            _exit(EXIT_FAILURE);
        }

        _exit(EXIT_SUCCESS);
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

    if (WEXITSTATUS(status)) {
        error_setg(errp, "child process has failed to suspend");
        goto out;
    }

out:
    g_free(pmutils_path);
}