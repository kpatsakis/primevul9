void address_space_destroy(AddressSpace *as)
{
    MemoryRegion *root = as->root;

    /* Flush out anything from MemoryListeners listening in on this */
    memory_region_transaction_begin();
    as->root = NULL;
    memory_region_transaction_commit(root);
    QTAILQ_REMOVE(&as->uc->address_spaces, as, address_spaces_link);

    /* At this point, as->dispatch and as->current_map are dummy
     * entries that the guest should never use.  Wait for the old
     * values to expire before freeing the data.
     */
    as->root = root;
    flatview_unref(as->current_map);
}