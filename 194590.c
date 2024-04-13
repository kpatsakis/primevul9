static uint64_t ioport_read(void *opaque, hwaddr addr,
                            unsigned size)
{
    PCIQXLDevice *qxl = opaque;

    trace_qxl_io_read_unexpected(qxl->id);
    return 0xff;
}