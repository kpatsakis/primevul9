static MemTxResult gic_dist_write(void *opaque, hwaddr offset, uint64_t data,
                                  unsigned size, MemTxAttrs attrs)
{
    trace_gic_dist_write(offset, size, data);

    switch (size) {
    case 1:
        gic_dist_writeb(opaque, offset, data, attrs);
        return MEMTX_OK;
    case 2:
        gic_dist_writew(opaque, offset, data, attrs);
        return MEMTX_OK;
    case 4:
        gic_dist_writel(opaque, offset, data, attrs);
        return MEMTX_OK;
    default:
        return MEMTX_ERROR;
    }
}