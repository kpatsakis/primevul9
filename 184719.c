void stl_phys_notdirty(AddressSpace *as, hwaddr addr, uint32_t val)
{
    address_space_stl_notdirty(as, addr, val, MEMTXATTRS_UNSPECIFIED, NULL);
}