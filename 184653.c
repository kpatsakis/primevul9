uint32_t ldl_be_phys(AddressSpace *as, hwaddr addr)
{
    return address_space_ldl_be(as, addr, MEMTXATTRS_UNSPECIFIED, NULL);
}