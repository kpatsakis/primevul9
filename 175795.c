gx_default_dev_spec_op(gx_device *pdev, int dev_spec_op, void *data, int size)
{
    switch(dev_spec_op) {
        case gxdso_form_begin:
        case gxdso_form_end:
        case gxdso_pattern_can_accum:
        case gxdso_pattern_start_accum:
        case gxdso_pattern_finish_accum:
        case gxdso_pattern_load:
        case gxdso_pattern_shading_area:
        case gxdso_pattern_is_cpath_accum:
        case gxdso_pattern_handles_clip_path:
        case gxdso_is_pdf14_device:
        case gxdso_supports_devn:
        case gxdso_supports_hlcolor:
        case gxdso_supports_saved_pages:
        case gxdso_needs_invariant_palette:
        case gxdso_supports_iccpostrender:
            return 0;
        case gxdso_pattern_shfill_doesnt_need_path:
            return (dev_proc(pdev, fill_path) == gx_default_fill_path);
        case gxdso_is_std_cmyk_1bit:
            return (dev_proc(pdev, map_cmyk_color) == cmyk_1bit_map_cmyk_color);
        case gxdso_interpolate_antidropout:
            return pdev->color_info.use_antidropout_downscaler;
        case gxdso_interpolate_threshold:
            if ((pdev->color_info.num_components == 1 &&
                 pdev->color_info.max_gray < 15) ||
                (pdev->color_info.num_components > 1 &&
                 pdev->color_info.max_color < 15)) {
                /* If we are a limited color device (i.e. we are halftoning)
                 * then only interpolate if we are upscaling by at least 4 */
                return 4;
            }
            return 0; /* Otherwise no change */
        case gxdso_get_dev_param:
            {
                dev_param_req_t *request = (dev_param_req_t *)data;
                return gx_default_get_param(pdev, request->Param, request->list);
            }
    }
    return_error(gs_error_undefined);
}