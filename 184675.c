void stw_be_phys(AddressSpace *as, hwaddr addr, uint32_t val)
{
    address_space_stw_be(as, addr, val, MEMTXATTRS_UNSPECIFIED, NULL);
}