gs_pattern2_make_pattern(gs_client_color * pcc,
                         const gs_pattern_template_t * pcp,
                         const gs_matrix * pmat, gs_gstate * pgs,
                         gs_memory_t * mem)
{
    const gs_pattern2_template_t *ptemp =
        (const gs_pattern2_template_t *)pcp;
    int code = gs_make_pattern_common(pcc, pcp, pmat, pgs, mem,
                                      &st_pattern2_instance);
    gs_pattern2_instance_t *pinst;

    if (code < 0)
        return code;
    pinst = (gs_pattern2_instance_t *)pcc->pattern;
    pinst->templat = *ptemp;
    pinst->shfill = false;
    return 0;
}