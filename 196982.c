gx_dc_pattern2_equal(const gx_device_color * pdevc1,
                     const gx_device_color * pdevc2)
{
    return pdevc2->type == pdevc1->type &&
        pdevc1->ccolor.pattern == pdevc2->ccolor.pattern;
}