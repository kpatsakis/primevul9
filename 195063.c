static uint32_t rtl8139_TxStatus_TxAddr_read(RTL8139State *s, uint32_t regs[],
                                             uint32_t base, uint8_t addr,
                                             int size)
{
    uint32_t reg = (addr - base) / 4;
    uint32_t offset = addr & 0x3;
    uint32_t ret = 0;

    if (addr & (size - 1)) {
        DPRINTF("not implemented read for TxStatus/TxAddr "
                "addr=0x%x size=0x%x\n", addr, size);
        return ret;
    }

    switch (size) {
    case 1: /* fall through */
    case 2: /* fall through */
    case 4:
        ret = (regs[reg] >> offset * 8) & (((uint64_t)1 << (size * 8)) - 1);
        DPRINTF("TxStatus/TxAddr[%d] read addr=0x%x size=0x%x val=0x%08x\n",
                reg, addr, size, ret);
        break;
    default:
        DPRINTF("unsupported size 0x%x of TxStatus/TxAddr reading\n", size);
        break;
    }

    return ret;
}