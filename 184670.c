void stl_phys(AddressSpace *as, hwaddr addr, uint32_t val)
{
    address_space_stl(as, addr, val, MEMTXATTRS_UNSPECIFIED, NULL);
}