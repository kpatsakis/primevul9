qmp_guest_set_memory_blocks(GuestMemoryBlockList *mem_blks, Error **errp)
{
    error_setg(errp, QERR_UNSUPPORTED);
    return NULL;
}