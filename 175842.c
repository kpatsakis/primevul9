int epo_get_bits(gx_device *dev, int y, byte *data, byte **actual_data)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, get_bits)(dev, y, data, actual_data);
}