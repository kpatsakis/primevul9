gx_default_output_page(gx_device * dev, int num_copies, int flush)
{
    int code = dev_proc(dev, sync_output)(dev);

    if (code >= 0)
        code = gx_finish_output_page(dev, num_copies, flush);
    return code;
}