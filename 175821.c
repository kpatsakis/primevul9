int epo_output_page(gx_device *dev, int num_copies, int flush)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, output_page)(dev, num_copies, flush);
}