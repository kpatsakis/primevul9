void qmp_guest_suspend_ram(Error **errp)
{
    Error *local_err = NULL;
    GuestSuspendMode *mode = g_new(GuestSuspendMode, 1);

    *mode = GUEST_SUSPEND_MODE_RAM;
    check_suspend_mode(*mode, &local_err);
    acquire_privilege(SE_SHUTDOWN_NAME, &local_err);
    execute_async(do_suspend, mode, &local_err);

    if (local_err) {
        error_propagate(errp, local_err);
        g_free(mode);
    }
}