void stw_phys(AddressSpace *as, hwaddr addr, uint32_t val)
{
    address_space_stw(as, addr, val, MEMTXATTRS_UNSPECIFIED, NULL);
}