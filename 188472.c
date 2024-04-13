void qmp_guest_suspend_ram(Error **errp)
{
    Error *local_err = NULL;

    bios_supports_mode("pm-is-supported", "--suspend", "mem", &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return;
    }

    guest_suspend("pm-suspend", "mem", errp);
}