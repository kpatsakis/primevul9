is_spotan_device(gx_device * dev)
{
    /* Use open_device procedure to identify the type of the device
     * instead of the standard gs_object_type() because gs_cpath_accum_device
     * is allocaded on the stack i.e. has no block header with a descriptor
     * but has dev->memory set like a heap-allocated device.
     */
    return dev->procs.open_device == san_open;
}