uint32_t address_space_ldl(AddressSpace *as, hwaddr addr,
                           MemTxAttrs attrs, MemTxResult *result)
{
    return address_space_ldl_internal(as, addr, attrs, result,
                                      DEVICE_NATIVE_ENDIAN);
}