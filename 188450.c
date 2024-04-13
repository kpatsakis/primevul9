int64_t qmp_guest_fsfreeze_thaw(Error **errp)
{
    int i;

    if (!vss_initialized()) {
        error_setg(errp, QERR_UNSUPPORTED);
        return 0;
    }

    qga_vss_fsfreeze(&i, false, errp);

    ga_unset_frozen(ga_state);
    return i;
}