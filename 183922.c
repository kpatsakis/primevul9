ssize_t vnc_client_write_buf(VncState *vs, const uint8_t *data, size_t datalen)
{
    Error *err = NULL;
    ssize_t ret;
    ret = qio_channel_write(
        vs->ioc, (const char *)data, datalen, &err);
    VNC_DEBUG("Wrote wire %p %zd -> %ld\n", data, datalen, ret);
    return vnc_client_io_error(vs, ret, &err);
}