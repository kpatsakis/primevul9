uint64_t address_space_ldq_be(AddressSpace *as, hwaddr addr,
                           MemTxAttrs attrs, MemTxResult *result)
{
    return address_space_ldq_internal(as, addr, attrs, result,
                                      DEVICE_BIG_ENDIAN);
}