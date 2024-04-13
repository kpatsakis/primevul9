pdf14_decrement_smask_color(gs_gstate * pgs, gx_device * dev)
{
    pdf14_device * pdev = (pdf14_device *) dev;
    pdf14_smaskcolor_t *smaskcolor = pdev->smaskcolor;
    gsicc_manager_t *icc_manager = pgs->icc_manager;
    int k;

    /* See comment in pdf14_increment_smask_color to understand this one */
    if (pdev->smaskcolor == NULL && pgs->icc_manager->smask_profiles != NULL &&
        pgs->icc_manager->smask_profiles->swapped) {
            return 0;
    }
    if (smaskcolor != NULL) {
        smaskcolor->ref_count--;
        if_debug1m(gs_debug_flag_icc, pgs->memory,
                   "[icc] Decrement smask color.  Now %d\n",
                   smaskcolor->ref_count);
        if (smaskcolor->ref_count == 0) {
            if_debug0m(gs_debug_flag_icc, pgs->memory, "[icc] Reset smask color.\n");
            /* Lets return the profiles and clean up */
            /* First see if we need to "reset" the profiles that are in
               the graphic state */
            if_debug0m(gs_debug_flag_icc, pgs->memory, "[icc] Reseting graphic state color spaces\n");
            for (k = 0; k < 2; k++) {
                gs_color_space *pcs = pgs->color[k].color_space;
                cmm_profile_t  *profile = pcs->cmm_icc_profile_data;
                if (profile != NULL) {
                    switch(profile->data_cs) {
                        case gsGRAY:
                            if (profile->hashcode ==
                                pgs->icc_manager->default_gray->hashcode) {
                                    profile =
                                        smaskcolor->profiles->smask_gray;
                            }
                            break;
                        case gsRGB:
                            if (profile->hashcode ==
                                pgs->icc_manager->default_rgb->hashcode) {
                                    profile =
                                        smaskcolor->profiles->smask_rgb;
                            }
                            break;
                        case gsCMYK:
                            if (profile->hashcode ==
                                pgs->icc_manager->default_cmyk->hashcode) {
                                    profile =
                                        smaskcolor->profiles->smask_cmyk;
                            }
                            break;
                        default:

                            break;
                    }
                    rc_assign(pcs->cmm_icc_profile_data, profile,
                              "pdf14_decrement_smask_color");
                }
            }

            icc_manager->default_gray = smaskcolor->profiles->smask_gray;
            icc_manager->default_rgb = smaskcolor->profiles->smask_rgb;
            icc_manager->default_cmyk = smaskcolor->profiles->smask_cmyk;
            icc_manager->smask_profiles->swapped = false;
            /* We didn't increment the reference count when we assigned these
             * so NULL them to avoid decrementing when smaskcolor is freed
             */
            smaskcolor->profiles->smask_gray =
              smaskcolor->profiles->smask_rgb =
              smaskcolor->profiles->smask_cmyk = NULL;

            pdf14_free_smask_color(pdev);
        }
    }
    return(0);
}