pass_shading_area_through_clip_path_device(gx_device * pdev, const gs_gstate * pgs,
                     gx_path * ppath, const gx_fill_params * params,
                 const gx_device_color * pdevc, const gx_clip_path * pcpath)
{
    if (pdevc == NULL) {
        gx_device_clip *cdev = (gx_device_clip *)pdev;

        return dev_proc(cdev->target, fill_path)(cdev->target, pgs, ppath, params, pdevc, pcpath);
    }
    /* We know that tha clip path device implements fill_path with default proc. */
    return gx_default_fill_path(pdev, pgs, ppath, params, pdevc, pcpath);
}