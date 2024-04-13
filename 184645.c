void stw_le_phys(AddressSpace *as, hwaddr addr, uint32_t val)
{
    address_space_stw_le(as, addr, val, MEMTXATTRS_UNSPECIFIED, NULL);
}