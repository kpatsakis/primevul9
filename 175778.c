gx_default_composite_clist_write_update(const gs_composite_t *pcte, gx_device * dev,
                gx_device ** pcdev, gs_gstate * pgs, gs_memory_t * mem)
{
    *pcdev = dev;		/* Do nothing -> return the same device */
    return 0;
}