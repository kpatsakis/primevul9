gx_null_create_compositor(gx_device * dev, gx_device ** pcdev,
                          const gs_composite_t * pcte,
                          gs_gstate * pgs, gs_memory_t * memory,
                          gx_device *cdev)
{
    *pcdev = dev;
    return 0;
}