uint64_t ldq_le_phys(AddressSpace *as, hwaddr addr)
{
    return address_space_ldq_le(as, addr, MEMTXATTRS_UNSPECIFIED, NULL);
}