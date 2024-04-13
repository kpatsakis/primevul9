image_skip_color_icc_tpr(gx_image_enum *penum, gx_device *dev)
{
    transform_pixel_region_data data;
    data.state = penum->tpr_state;
    return dev_proc(dev, transform_pixel_region)(dev, transform_pixel_region_data_needed, &data) == 0;
}