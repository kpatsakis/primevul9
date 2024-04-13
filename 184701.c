void stq_le_phys(AddressSpace *as, hwaddr addr, uint64_t val)
{
    address_space_stq_le(as, addr, val, MEMTXATTRS_UNSPECIFIED, NULL);
}