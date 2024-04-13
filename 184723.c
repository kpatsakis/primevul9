void address_space_stq(AddressSpace *as, hwaddr addr, uint64_t val,
                       MemTxAttrs attrs, MemTxResult *result)
{
    MemTxResult r;
    val = tswap64(val);
    r = address_space_rw(as, addr, attrs, (void *) &val, 8, 1);
    if (result) {
        *result = r;
    }
}