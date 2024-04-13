gx_dc_pattern2_save_dc(
    const gx_device_color * pdevc,
    gx_device_color_saved * psdc )
{
    gs_pattern2_instance_t * pinst = (gs_pattern2_instance_t *)pdevc->ccolor.pattern;

    psdc->type = pdevc->type;
    psdc->colors.pattern2.id = pinst->pattern_id;
    psdc->colors.pattern2.shfill = pinst->shfill;
}