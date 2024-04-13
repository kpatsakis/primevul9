void stq_be_phys(AddressSpace *as, hwaddr addr, uint64_t val)
{
    address_space_stq_be(as, addr, val, MEMTXATTRS_UNSPECIFIED, NULL);
}