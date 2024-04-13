gx_default_create_compositor(gx_device * dev, gx_device ** pcdev,
                             const gs_composite_t * pcte,
                             gs_gstate * pgs, gs_memory_t * memory,
                             gx_device *cdev)
{
    return pcte->type->procs.create_default_compositor
        (pcte, pcdev, dev, pgs, memory);
}