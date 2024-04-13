void address_space_stl(AddressSpace *as, hwaddr addr, uint32_t val,
                       MemTxAttrs attrs, MemTxResult *result)
{
    address_space_stl_internal(as, addr, val, attrs, result,
                               DEVICE_NATIVE_ENDIAN);
}