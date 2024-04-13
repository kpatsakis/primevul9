MemTxResult address_space_read(AddressSpace *as, hwaddr addr, MemTxAttrs attrs,
                               uint8_t *buf, int len)
{
    return address_space_rw(as, addr, attrs, buf, len, false);
}