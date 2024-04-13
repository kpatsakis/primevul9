GuestMemoryBlockInfo *qmp_guest_get_memory_block_info(Error **errp)
{
    error_setg(errp, QERR_UNSUPPORTED);
    return NULL;
}