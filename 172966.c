pdf14_recreate_device(gs_memory_t *mem,	gs_gstate	* pgs,
                gx_device * dev, const gs_pdf14trans_t * pdf14pct)
{
    pdf14_device * pdev = (pdf14_device *)dev;
    gx_device * target = pdev->target;
    pdf14_device * dev_proto;
    pdf14_device temp_dev_proto;
    bool has_tags = dev->graphics_type_tag & GS_DEVICE_ENCODES_TAGS;
    int code;

    if_debug0m('v', dev->memory, "[v]pdf14_recreate_device\n");

    /*
     * We will not use the entire prototype device but we will set the
     * color related info and the device procs to match the prototype.
     */
    code = get_pdf14_device_proto(target, &dev_proto, &temp_dev_proto, pgs,
                                  pdf14pct, false);
    if (code < 0)
        return code;
    pdev->color_info = dev_proto->color_info;
    pdev->pad = target->pad;
    pdev->log2_align_mod = target->log2_align_mod;
    pdev->is_planar = target->is_planar;
    pdev->procs = dev_proto->procs;
    if (has_tags) {
        pdev->procs.encode_color = pdf14_encode_color_tag;
        pdev->color_info.depth += 8;
    }
    dev->static_procs = dev_proto->static_procs;
    gx_device_set_procs(dev);
    gx_device_fill_in_procs(dev);
    check_device_separable(dev);
    return dev_proc(pdev, open_device)(dev);
}