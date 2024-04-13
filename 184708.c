void address_space_stq_be(AddressSpace *as, hwaddr addr, uint64_t val,
                       MemTxAttrs attrs, MemTxResult *result)
{
    MemTxResult r;
    val = cpu_to_be64(val);
    r = address_space_rw(as, addr, attrs, (void *) &val, 8, 1);
    if (result) {
        *result = r;
    }
}