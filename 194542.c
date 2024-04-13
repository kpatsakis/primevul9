static void interface_attach_worker(QXLInstance *sin, QXLWorker *qxl_worker)
{
    PCIQXLDevice *qxl = container_of(sin, PCIQXLDevice, ssd.qxl);

    trace_qxl_interface_attach_worker(qxl->id);
}