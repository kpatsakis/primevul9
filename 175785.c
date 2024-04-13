gx_default_end_page(gx_device * dev, int reason, gs_gstate * pgs)
{
    return (reason != 2 ? 1 : 0);
}