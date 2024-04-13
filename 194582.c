static void display_switch(DisplayChangeListener *dcl,
                           struct DisplaySurface *surface)
{
    PCIQXLDevice *qxl = container_of(dcl, PCIQXLDevice, ssd.dcl);

    qxl->ssd.ds = surface;
    if (qxl->mode == QXL_MODE_VGA) {
        qemu_spice_display_switch(&qxl->ssd, surface);
    }
}