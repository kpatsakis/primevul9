static void qxl_update_irq_bh(void *opaque)
{
    PCIQXLDevice *d = opaque;
    qxl_update_irq(d);
}