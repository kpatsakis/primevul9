gx_dc_pattern2_can_overlap(const gx_device_color *pdevc)
{
    gs_pattern2_instance_t * pinst;

    if (pdevc->type != &gx_dc_pattern2)
        return false;
    pinst = (gs_pattern2_instance_t *)pdevc->ccolor.pattern;
    switch (pinst->templat.Shading->head.type) {
        case 3: case 6: case 7:
            return true;
        default:
            return false;
    }
}