static int qxl_pre_load(void *opaque)
{
    PCIQXLDevice* d = opaque;

    trace_qxl_pre_load(d->id);
    qxl_hard_reset(d, 1);
    qxl_exit_vga_mode(d);
    return 0;
}