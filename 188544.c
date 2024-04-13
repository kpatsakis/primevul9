GuestFsfreezeStatus qmp_guest_fsfreeze_status(Error **errp)
{
    if (ga_is_frozen(ga_state)) {
        return GUEST_FSFREEZE_STATUS_FROZEN;
    }

    return GUEST_FSFREEZE_STATUS_THAWED;
}