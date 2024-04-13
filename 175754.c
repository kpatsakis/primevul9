is_like_DeviceCMYK(gx_device * dev)
{
    subclass_color_mappings         scm;
    frac                            cm_comp_fracs[4];
    int                             i;

    if ( dev->color_info.num_components != 4                      ||
         dev->color_info.polarity != GX_CINFO_POLARITY_SUBTRACTIVE  )
        return false;
    scm = get_color_mapping_procs_subclass(dev);

    /* check the values 1/4, 1/3, 3/4, and 1/8 */

    map_cmyk_subclass( scm,
                        frac_1 / 4,
                        frac_1 / 3,
                        3 * frac_1 / 4,
                        frac_1 / 8,
                        cm_comp_fracs );

    /* verify results to .01 */
    cm_comp_fracs[0] -= frac_1 / 4;
    cm_comp_fracs[1] -= frac_1 / 3;
    cm_comp_fracs[2] -= 3 * frac_1 / 4;
    cm_comp_fracs[3] -= frac_1 / 8;
    for ( i = 0;
           i < 4                            &&
           -frac_1 / 100 < cm_comp_fracs[i] &&
           cm_comp_fracs[i] < frac_1 / 100;
          i++ )
        ;
    return i == 4;
}