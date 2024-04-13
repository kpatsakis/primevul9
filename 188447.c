GuestFsfreezeStatus qmp_guest_fsfreeze_status(Error **errp)
{
    if (!vss_initialized()) {
        error_setg(errp, QERR_UNSUPPORTED);
        return 0;
    }

    if (ga_is_frozen(ga_state)) {
        return GUEST_FSFREEZE_STATUS_FROZEN;
    }

    return GUEST_FSFREEZE_STATUS_THAWED;
}