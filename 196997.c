gx_dc_pattern2_get_color_space(const gx_device_color * pdevc)
{
    gs_pattern2_instance_t *pinst =
        (gs_pattern2_instance_t *)pdevc->ccolor.pattern;
    const gs_shading_t *psh = pinst->templat.Shading;

    return psh->params.ColorSpace;
}