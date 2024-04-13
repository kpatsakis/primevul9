gs_pdf14_device_copy_params(gx_device *dev, const gx_device *target)
{
    cmm_dev_profile_t *profile_targ;
    cmm_dev_profile_t *profile_dev14;
    pdf14_device *pdev = (pdf14_device*) dev;

    COPY_PARAM(width);
    COPY_PARAM(height);
    COPY_ARRAY_PARAM(MediaSize);
    COPY_ARRAY_PARAM(ImagingBBox);
    COPY_PARAM(ImagingBBox_set);
    COPY_ARRAY_PARAM(HWResolution);
    COPY_ARRAY_PARAM(Margins);
    COPY_ARRAY_PARAM(HWMargins);
    COPY_PARAM(PageCount);
    COPY_PARAM(MaxPatternBitmap);
    memcpy(&(dev->space_params), &(target->space_params), sizeof(gdev_space_params));
    /* The PDF14 device copies only the default profile not the text etc.
       TODO: MJV.  It has to make its own device structure but
       can grab a copy of the profile.  This allows swapping of profiles
       in the PDF14 device without messing up the target device profile.
       Also if the device is using a blend color space it will grab that too */
    if (dev->icc_struct == NULL) {
        dev->icc_struct = gsicc_new_device_profile_array(dev->memory);
        profile_dev14 = dev->icc_struct;
        dev_proc((gx_device *) target, get_profile)((gx_device *) target,
                                          &(profile_targ));
        profile_dev14->device_profile[0] = profile_targ->device_profile[0];
        dev->icc_struct->devicegraytok = profile_targ->devicegraytok;
        dev->icc_struct->graydetection = profile_targ->graydetection;
        dev->icc_struct->pageneutralcolor = profile_targ->pageneutralcolor;
        dev->icc_struct->supports_devn = profile_targ->supports_devn;
        dev->icc_struct->usefastcolor = profile_targ->usefastcolor;
        gx_monitor_enter(profile_dev14->device_profile[0]->lock);
        rc_increment(profile_dev14->device_profile[0]);
        gx_monitor_leave(profile_dev14->device_profile[0]->lock);
        profile_dev14->rendercond[0] = profile_targ->rendercond[0];
        if (pdev->using_blend_cs) {
            /* Swap the device profile and the blend profile. */
            profile_dev14->blend_profile = profile_targ->device_profile[0];
            profile_dev14->device_profile[0] = profile_targ->blend_profile;
            gx_monitor_enter(profile_dev14->device_profile[0]->lock);
            rc_increment(profile_dev14->device_profile[0]);
            gx_monitor_leave(profile_dev14->device_profile[0]->lock);
        }
        profile_dev14->sim_overprint = profile_targ->sim_overprint;
    }
    dev->graphics_type_tag = target->graphics_type_tag;	/* initialize to same as target */
#undef COPY_ARRAY_PARAM
#undef COPY_PARAM
}