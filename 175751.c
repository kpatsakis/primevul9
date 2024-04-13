gx_default_open_device(gx_device * dev)
{
    /* Initialize the separable status if not known. */
    check_device_separable(dev);
    return 0;
}