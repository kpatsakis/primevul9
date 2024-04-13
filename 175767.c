int epo_process_page(gx_device *dev, gx_process_page_options_t *options)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, process_page)(dev, options);
}