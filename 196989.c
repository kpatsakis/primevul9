bool gx_dc_pattern2_has_background(const gx_device_color *pdevc)
{
    gs_pattern2_instance_t * pinst;
    const gs_shading_t *Shading;

    if (pdevc->type != &gx_dc_pattern2)
        return false;
    pinst = (gs_pattern2_instance_t *)pdevc->ccolor.pattern;
    Shading = pinst->templat.Shading;
    return !pinst->shfill && Shading->params.Background != NULL;
}