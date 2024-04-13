static int interface_flush_resources(QXLInstance *sin)
{
    PCIQXLDevice *qxl = container_of(sin, PCIQXLDevice, ssd.qxl);
    int ret;

    ret = qxl->num_free_res;
    if (ret) {
        qxl_push_free_res(qxl, 1);
    }
    return ret;
}