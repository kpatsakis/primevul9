uint32_t lduw_be_phys(AddressSpace *as, hwaddr addr)
{
    return address_space_lduw_be(as, addr, MEMTXATTRS_UNSPECIFIED, NULL);
}