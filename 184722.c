uint32_t ldl_le_phys(AddressSpace *as, hwaddr addr)
{
    return address_space_ldl_le(as, addr, MEMTXATTRS_UNSPECIFIED, NULL);
}