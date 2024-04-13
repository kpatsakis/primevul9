ssize_t vnc_client_read_buf(VncState *vs, uint8_t *data, size_t datalen)
{
    ssize_t ret;
    Error *err = NULL;
    ret = qio_channel_read(
        vs->ioc, (char *)data, datalen, &err);
    VNC_DEBUG("Read wire %p %zd -> %ld\n", data, datalen, ret);
    return vnc_client_io_error(vs, ret, &err);
}