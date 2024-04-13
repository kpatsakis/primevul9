gx_get_largest_clipping_box(gx_device * dev, gs_fixed_rect * pbox)
{
    pbox->p.x = min_fixed;
    pbox->p.y = min_fixed;
    pbox->q.x = max_fixed;
    pbox->q.y = max_fixed;
}