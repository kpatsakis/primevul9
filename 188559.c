void qmp_guest_suspend_disk(Error **errp)
{
    Error *local_err = NULL;

    bios_supports_mode("pm-is-supported", "--hibernate", "disk", &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return;
    }

    guest_suspend("pm-hibernate", "disk", errp);
}