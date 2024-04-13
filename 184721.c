void stl_be_phys(AddressSpace *as, hwaddr addr, uint32_t val)
{
    address_space_stl_be(as, addr, val, MEMTXATTRS_UNSPECIFIED, NULL);
}