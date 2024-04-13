pdf14_ok_to_optimize(gx_device *dev)
{
    bool using_blend_cs;
    pdf14_default_colorspace_t pdf14_cs =
        pdf14_determine_default_blend_cs(dev, false, &using_blend_cs);
    gsicc_colorbuffer_t dev_icc_cs;
    bool ok = false;
    int tag_depth = (dev->graphics_type_tag & GS_DEVICE_ENCODES_TAGS) ? 8 : 0;
    cmm_dev_profile_t *dev_profile;
    int code = dev_proc(dev, get_profile)(dev,  &dev_profile);

    if (code < 0)
        return false;
    dev_icc_cs = dev_profile->device_profile[0]->data_cs;
    /* If the outputprofile is not "standard" then colors converted to device color */
    /* during clist writing won't match the colors written for the pdf14 clist dev  */
    if (!(dev_icc_cs == gsGRAY || dev_icc_cs == gsRGB || dev_icc_cs == gsCMYK) || using_blend_cs)
        return false;                           /* can't handle funky output profiles */

    switch (pdf14_cs) {
        case PDF14_DeviceGray:
            ok = dev->color_info.max_gray == 255 && dev->color_info.depth == 8 + tag_depth;
            break;
        case PDF14_DeviceRGB:
            ok = dev->color_info.max_color == 255 && dev->color_info.depth == 24 + tag_depth;
            break;
        case PDF14_DeviceCMYK:
            ok = dev->color_info.max_color == 255 && dev->color_info.depth == 32 + tag_depth;
            break;
        case PDF14_DeviceCMYKspot:
            ok = false;			/* punt for this case */
            break;
        case PDF14_DeviceCustom:
            /*
             * We are using the output device's process color model.  The
             * color_info for the PDF 1.4 compositing device needs to match
             * the output device, but it may not have been contone.
             */
            ok = dev->color_info.depth == dev->color_info.num_components * 8 + tag_depth;
            break;
        default:			/* Should not occur */
            ok = false;
    }
    return ok;
}