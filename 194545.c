static void display_refresh(DisplayChangeListener *dcl)
{
    PCIQXLDevice *qxl = container_of(dcl, PCIQXLDevice, ssd.dcl);

    if (qxl->mode == QXL_MODE_VGA) {
        qemu_spice_display_refresh(&qxl->ssd);
    }
}