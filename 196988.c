gx_dc_pattern2_get_dev_halftone(const gx_device_color * pdevc)
{
    return ((gs_pattern2_instance_t *)pdevc->ccolor.pattern)->saved->dev_ht;
}