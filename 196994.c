gx_dc_pattern2_shade_bbox_transform2fixed(const gs_rect * rect, const gs_gstate * pgs,
                           gs_fixed_rect * rfixed)
{
    gs_rect dev_rect;
    int code = gs_bbox_transform(rect, &ctm_only(pgs), &dev_rect);

    if (code >= 0) {
        rfixed->p.x = float2fixed(dev_rect.p.x);
        rfixed->p.y = float2fixed(dev_rect.p.y);
        rfixed->q.x = float2fixed(dev_rect.q.x);
        rfixed->q.y = float2fixed(dev_rect.q.y);
    }
    return code;
}