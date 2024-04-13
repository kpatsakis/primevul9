void stq_phys(AddressSpace *as, hwaddr addr, uint64_t val)
{
    address_space_stq(as, addr, val, MEMTXATTRS_UNSPECIFIED, NULL);
}