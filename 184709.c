uint32_t address_space_ldl_be(AddressSpace *as, hwaddr addr,
                              MemTxAttrs attrs, MemTxResult *result)
{
    return address_space_ldl_internal(as, addr, attrs, result,
                                      DEVICE_BIG_ENDIAN);
}