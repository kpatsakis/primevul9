static void qxl_hw_update(void *opaque)
{
    PCIQXLDevice *qxl = opaque;

    qxl_render_update(qxl);
}