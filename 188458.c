void qmp_guest_set_user_password(const char *username,
                                 const char *password,
                                 bool crypted,
                                 Error **errp)
{
    Error *local_err = NULL;
    char *passwd_path = NULL;
    pid_t pid;
    int status;
    int datafd[2] = { -1, -1 };
    char *rawpasswddata = NULL;
    size_t rawpasswdlen;
    char *chpasswddata = NULL;
    size_t chpasswdlen;

    rawpasswddata = (char *)qbase64_decode(password, -1, &rawpasswdlen, errp);
    if (!rawpasswddata) {
        return;
    }
    rawpasswddata = g_renew(char, rawpasswddata, rawpasswdlen + 1);
    rawpasswddata[rawpasswdlen] = '\0';

    if (strchr(rawpasswddata, '\n')) {
        error_setg(errp, "forbidden characters in raw password");
        goto out;
    }

    if (strchr(username, '\n') ||
        strchr(username, ':')) {
        error_setg(errp, "forbidden characters in username");
        goto out;
    }

    chpasswddata = g_strdup_printf("%s:%s\n", username, rawpasswddata);
    chpasswdlen = strlen(chpasswddata);

    passwd_path = g_find_program_in_path("chpasswd");

    if (!passwd_path) {
        error_setg(errp, "cannot find 'passwd' program in PATH");
        goto out;
    }

    if (pipe(datafd) < 0) {
        error_setg(errp, "cannot create pipe FDs");
        goto out;
    }

    pid = fork();
    if (pid == 0) {
        close(datafd[1]);
        /* child */
        setsid();
        dup2(datafd[0], 0);
        reopen_fd_to_null(1);
        reopen_fd_to_null(2);

        if (crypted) {
            execle(passwd_path, "chpasswd", "-e", NULL, environ);
        } else {
            execle(passwd_path, "chpasswd", NULL, environ);
        }
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        error_setg_errno(errp, errno, "failed to create child process");
        goto out;
    }
    close(datafd[0]);
    datafd[0] = -1;

    if (qemu_write_full(datafd[1], chpasswddata, chpasswdlen) != chpasswdlen) {
        error_setg_errno(errp, errno, "cannot write new account password");
        goto out;
    }
    close(datafd[1]);
    datafd[1] = -1;

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
        error_setg(errp, "child process has failed to set user password");
        goto out;
    }

out:
    g_free(chpasswddata);
    g_free(rawpasswddata);
    g_free(passwd_path);
    if (datafd[0] != -1) {
        close(datafd[0]);
    }
    if (datafd[1] != -1) {
        close(datafd[1]);
    }
}