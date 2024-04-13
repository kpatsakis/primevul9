static void pdf14_cleanup_parent_color_profiles (pdf14_device *pdev)
{
    if (pdev->ctx) {
        pdf14_buf *buf, *next;

        for (buf = pdev->ctx->stack; buf != NULL; buf = next) {
            pdf14_parent_color_t *old_parent_color_info = buf->parent_color_info_procs;
            next = buf->saved;
            while (old_parent_color_info) {
               if (old_parent_color_info->icc_profile != NULL) {
                   cmm_profile_t *group_profile;
                   gsicc_rendering_param_t render_cond;
                   cmm_dev_profile_t *dev_profile;
                   int code = dev_proc((gx_device *)pdev, get_profile)((gx_device *)pdev,  &dev_profile);

                   if (code >= 0) {
                       gsicc_extract_profile(GS_UNKNOWN_TAG, dev_profile, &group_profile,
                                             &render_cond);

                       rc_decrement(group_profile,"pdf14_end_transparency_group");
                       pdev->icc_struct->device_profile[0] = old_parent_color_info->icc_profile;
                       rc_decrement(old_parent_color_info->icc_profile,"pdf14_end_transparency_group");
                       old_parent_color_info->icc_profile = NULL;
                   }
               }

               old_parent_color_info = old_parent_color_info->previous;
            }
        }
    }
}