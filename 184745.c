uint32_t ldub_phys(AddressSpace *as, hwaddr addr)
{
    return address_space_ldub(as, addr, MEMTXATTRS_UNSPECIFIED, NULL);
}