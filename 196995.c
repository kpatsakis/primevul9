gs_pattern2_remap_color(const gs_client_color * pc, const gs_color_space * pcs,
                        gx_device_color * pdc, const gs_gstate * pgs,
                        gx_device * dev, gs_color_select_t select)
{
    /* We don't do any actual color mapping now. */
    pdc->type = &gx_dc_pattern2;
    pdc->ccolor = *pc;
    pdc->ccolor_valid = true;
    return 0;
}