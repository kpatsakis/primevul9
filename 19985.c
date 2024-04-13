void address_space_init(struct uc_struct *uc,
                        AddressSpace *as,
                        MemoryRegion *root)
{
    as->uc = uc;
    as->root = root;
    as->current_map = NULL;
    QTAILQ_INIT(&as->listeners);
    QTAILQ_INSERT_TAIL(&uc->address_spaces, as, address_spaces_link);
    address_space_update_topology(as);
}