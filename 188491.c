static void ga_wait_child(pid_t pid, int *status, Error **errp)
{
    pid_t rpid;

    *status = 0;

    do {
        rpid = waitpid(pid, status, 0);
    } while (rpid == -1 && errno == EINTR);

    if (rpid == -1) {
        error_setg_errno(errp, errno, "failed to wait for child (pid: %d)",
                         pid);
        return;
    }

    g_assert(rpid == pid);
}