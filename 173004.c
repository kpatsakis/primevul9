pdf14_recreate_clist_device(gs_memory_t	*mem, gs_gstate *	pgs,
                gx_device * dev, const gs_pdf14trans_t * pdf14pct)
{
    pdf14_clist_device * pdev = (pdf14_clist_device *)dev;
    gx_device * target = pdev->target;
    pdf14_clist_device * dev_proto;
    pdf14_clist_device temp_dev_proto;
    int code;

    if_debug0m('v', pgs->memory, "[v]pdf14_recreate_clist_device\n");
    /*
     * We will not use the entire prototype device but we will set the
     * color related info to match the prototype.
     */
    code = get_pdf14_clist_device_proto(target, &dev_proto,
                                 &temp_dev_proto, pgs, pdf14pct, false);
    if (code < 0)
        return code;
    pdev->color_info = dev_proto->color_info;
    pdev->procs = dev_proto->procs;
    pdev->pad = target->pad;
    pdev->log2_align_mod = target->log2_align_mod;
    pdev->is_planar = target->is_planar;
    gx_device_fill_in_procs(dev);
    check_device_separable((gx_device *)pdev);
    return code;
}