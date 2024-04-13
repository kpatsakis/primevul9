pdf14_pop_parent_color(gx_device *dev, const gs_gstate *pgs)
{
    pdf14_device *pdev = (pdf14_device *)dev;
    pdf14_parent_color_t *old_parent_color_info = pdev->trans_group_parent_cmap_procs;

    if_debug0m('v', dev->memory, "[v]pdf14_pop_parent_color\n");
    /* We need to compliment pdf14_push_parent color */
    if (old_parent_color_info->icc_profile != NULL)
        rc_decrement(old_parent_color_info->icc_profile, "pdf14_pop_parent_color");
    /* Update the link */
    pdev->trans_group_parent_cmap_procs = old_parent_color_info->previous;
    /* Free the old one */
    gs_free_object(dev->memory, old_parent_color_info, "pdf14_clr_free");
}