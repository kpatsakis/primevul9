gx_default_finish_copydevice(gx_device *dev, const gx_device *from_dev)
{
    /* Only allow copying the prototype. */
    return (from_dev->memory ? gs_note_error(gs_error_rangecheck) : 0);
}