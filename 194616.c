static void interface_notify_update(QXLInstance *sin, uint32_t update_id)
{
    /*
     * Called by spice-server as a result of a QXL_CMD_UPDATE which is not in
     * use by xf86-video-qxl and is defined out in the qxl windows driver.
     * Probably was at some earlier version that is prior to git start (2009),
     * and is still guest trigerrable.
     */
    fprintf(stderr, "%s: deprecated\n", __func__);
}