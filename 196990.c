gx_dc_shading_path_add_box(gx_path *ppath, const gx_device_color * pdevc)
{
    gs_pattern2_instance_t *pinst = (gs_pattern2_instance_t *)pdevc->ccolor.pattern;
    const gs_shading_t *psh = pinst->templat.Shading;

    if (!psh->params.have_BBox)
        return_error(gs_error_unregistered); /* Do not call in this case. */
    else {
        gs_gstate *pgs = pinst->saved;

        return gs_shading_path_add_box(ppath, &psh->params.BBox, &pgs->ctm);
    }
}