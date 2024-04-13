uint64_t ldq_phys(AddressSpace *as, hwaddr addr)
{
    return address_space_ldq(as, addr, MEMTXATTRS_UNSPECIFIED, NULL);
}