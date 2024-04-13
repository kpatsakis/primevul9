void memory_region_transaction_commit(MemoryRegion *mr)
{
    AddressSpace *as;

    if (mr->uc->memory_region_update_pending) {
        flatviews_reset(mr->uc);

        MEMORY_LISTENER_CALL_GLOBAL(mr->uc, begin, Forward);

        QTAILQ_FOREACH(as, &mr->uc->address_spaces, address_spaces_link) {
            address_space_set_flatview(as);
        }
        mr->uc->memory_region_update_pending = false;
        MEMORY_LISTENER_CALL_GLOBAL(mr->uc, commit, Forward);
    }
}