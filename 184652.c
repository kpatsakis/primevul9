uint32_t address_space_ldl_le(AddressSpace *as, hwaddr addr,
                              MemTxAttrs attrs, MemTxResult *result)
{
    return address_space_ldl_internal(as, addr, attrs, result,
                                      DEVICE_LITTLE_ENDIAN);
}