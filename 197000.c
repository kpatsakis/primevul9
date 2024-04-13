gx_dc_pattern2_get_bbox(const gx_device_color * pdevc, gs_fixed_rect *bbox)
{
    gs_pattern2_instance_t *pinst =
        (gs_pattern2_instance_t *)pdevc->ccolor.pattern;
    int code;

    if (!pinst->templat.Shading->params.have_BBox)
        return 0;
    code = gx_dc_pattern2_shade_bbox_transform2fixed(
                &pinst->templat.Shading->params.BBox, (gs_gstate *)pinst->saved, bbox);
    if (code < 0)
        return code;
    return 1;
}