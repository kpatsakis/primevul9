uint32_t ldl_phys(AddressSpace *as, hwaddr addr)
{
    return address_space_ldl(as, addr, MEMTXATTRS_UNSPECIFIED, NULL);
}