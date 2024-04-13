void stl_le_phys(AddressSpace *as, hwaddr addr, uint32_t val)
{
    address_space_stl_le(as, addr, val, MEMTXATTRS_UNSPECIFIED, NULL);
}