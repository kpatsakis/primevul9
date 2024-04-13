static MemTxResult gic_dist_read(void *opaque, hwaddr offset, uint64_t *data,
                                 unsigned size, MemTxAttrs attrs)
{
    switch (size) {
    case 1:
        *data = gic_dist_readb(opaque, offset, attrs);
        break;
    case 2:
        *data = gic_dist_readb(opaque, offset, attrs);
        *data |= gic_dist_readb(opaque, offset + 1, attrs) << 8;
        break;
    case 4:
        *data = gic_dist_readb(opaque, offset, attrs);
        *data |= gic_dist_readb(opaque, offset + 1, attrs) << 8;
        *data |= gic_dist_readb(opaque, offset + 2, attrs) << 16;
        *data |= gic_dist_readb(opaque, offset + 3, attrs) << 24;
        break;
    default:
        return MEMTX_ERROR;
    }

    trace_gic_dist_read(offset, size, *data);
    return MEMTX_OK;
}