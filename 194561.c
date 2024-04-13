static void qxl_create_guest_primary_complete(PCIQXLDevice *qxl)
{
    /* for local rendering */
    qxl_render_resize(qxl);
}