static void display_update(DisplayChangeListener *dcl,
                           int x, int y, int w, int h)
{
    PCIQXLDevice *qxl = container_of(dcl, PCIQXLDevice, ssd.dcl);

    if (qxl->mode == QXL_MODE_VGA) {
        qemu_spice_display_update(&qxl->ssd, x, y, w, h);
    }
}