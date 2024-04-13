uint32_t address_space_ldub(AddressSpace *as, hwaddr addr,
                            MemTxAttrs attrs, MemTxResult *result)
{
    uint8_t val;
    MemTxResult r;

    r = address_space_rw(as, addr, attrs, &val, 1, 0);
    if (result) {
        *result = r;
    }
    return val;
}