gs_pattern2_set_shfill(gs_client_color * pcc)
{
    gs_pattern2_instance_t *pinst;

    if (pcc->pattern->type != &gs_pattern2_type)
        return_error(gs_error_unregistered); /* Must not happen. */
    pinst = (gs_pattern2_instance_t *)pcc->pattern;
    pinst->shfill = true;
    return 0;
}