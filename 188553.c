int64_t qmp_guest_fsfreeze_freeze(Error **errp)
{
    return qmp_guest_fsfreeze_freeze_list(false, NULL, errp);
}