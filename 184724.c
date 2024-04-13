void stb_phys(AddressSpace *as, hwaddr addr, uint32_t val)
{
    address_space_stb(as, addr, val, MEMTXATTRS_UNSPECIFIED, NULL);
}