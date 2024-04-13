uint64_t ldq_be_phys(AddressSpace *as, hwaddr addr)
{
    return address_space_ldq_be(as, addr, MEMTXATTRS_UNSPECIFIED, NULL);
}