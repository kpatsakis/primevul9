void qmp_guest_shutdown(bool has_mode, const char *mode, Error **errp)
{
    Error *local_err = NULL;
    UINT shutdown_flag = EWX_FORCE;

    slog("guest-shutdown called, mode: %s", mode);

    if (!has_mode || strcmp(mode, "powerdown") == 0) {
        shutdown_flag |= EWX_POWEROFF;
    } else if (strcmp(mode, "halt") == 0) {
        shutdown_flag |= EWX_SHUTDOWN;
    } else if (strcmp(mode, "reboot") == 0) {
        shutdown_flag |= EWX_REBOOT;
    } else {
        error_setg(errp, QERR_INVALID_PARAMETER_VALUE, "mode",
                   "halt|powerdown|reboot");
        return;
    }

    /* Request a shutdown privilege, but try to shut down the system
       anyway. */
    acquire_privilege(SE_SHUTDOWN_NAME, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return;
    }

    if (!ExitWindowsEx(shutdown_flag, SHTDN_REASON_FLAG_PLANNED)) {
        slog("guest-shutdown failed: %lu", GetLastError());
        error_setg(errp, QERR_UNDEFINED_ERROR);
    }
}