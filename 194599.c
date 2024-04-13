static void interface_async_complete(QXLInstance *sin, uint64_t cookie_token)
{
    PCIQXLDevice *qxl = container_of(sin, PCIQXLDevice, ssd.qxl);
    QXLCookie *cookie = (QXLCookie *)(uintptr_t)cookie_token;

    switch (cookie->type) {
    case QXL_COOKIE_TYPE_IO:
        interface_async_complete_io(qxl, cookie);
        g_free(cookie);
        break;
    case QXL_COOKIE_TYPE_RENDER_UPDATE_AREA:
        qxl_render_update_area_done(qxl, cookie);
        break;
    case QXL_COOKIE_TYPE_POST_LOAD_MONITORS_CONFIG:
        break;
    default:
        fprintf(stderr, "qxl: %s: unexpected cookie type %d\n",
                __func__, cookie->type);
        g_free(cookie);
    }
}