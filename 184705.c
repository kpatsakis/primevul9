MemTxResult address_space_write(AddressSpace *as, hwaddr addr, MemTxAttrs attrs,
                                const uint8_t *buf, int len)
{
    return address_space_rw(as, addr, attrs, (uint8_t *)buf, len, true);
}