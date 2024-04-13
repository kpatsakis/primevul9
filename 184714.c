uint32_t lduw_phys(AddressSpace *as, hwaddr addr)
{
    return address_space_lduw(as, addr, MEMTXATTRS_UNSPECIFIED, NULL);
}