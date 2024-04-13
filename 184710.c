uint32_t address_space_lduw(AddressSpace *as, hwaddr addr,
                           MemTxAttrs attrs, MemTxResult *result)
{
    return address_space_lduw_internal(as, addr, attrs, result,
                                       DEVICE_NATIVE_ENDIAN);
}