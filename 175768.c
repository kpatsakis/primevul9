int epo_create_compositor(gx_device *dev, gx_device **pcdev, const gs_composite_t *pcte,
    gs_gstate *pgs, gs_memory_t *memory, gx_device *cdev)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, create_compositor)(dev, pcdev, pcte, pgs, memory, cdev);
}