uint32_t lduw_le_phys(AddressSpace *as, hwaddr addr)
{
    return address_space_lduw_le(as, addr, MEMTXATTRS_UNSPECIFIED, NULL);
}