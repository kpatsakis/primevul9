int gx_subclass_create_compositor(gx_device *dev, gx_device **pcdev, const gs_composite_t *pcte,
    gs_gstate *pgs, gs_memory_t *memory, gx_device *cdev)
{
    pdf14_clist_device *p14dev;
    generic_subclass_data *psubclass_data;
    int code = 0;

    p14dev = (pdf14_clist_device *)dev;
    psubclass_data = p14dev->target->subclass_data;

    set_dev_proc(dev, create_compositor, psubclass_data->saved_compositor_method);

    if (gs_is_pdf14trans_compositor(pcte) != 0 && strncmp(dev->dname, "pdf14clist", 10) == 0) {
        const gs_pdf14trans_t * pdf14pct = (const gs_pdf14trans_t *) pcte;

        switch (pdf14pct->params.pdf14_op) {
            case PDF14_POP_DEVICE:
                {
                    pdf14_clist_device *p14dev = (pdf14_clist_device *)dev;
                    gx_device *subclass_device;

                    p14dev->target->color_info = p14dev->saved_target_color_info;
                    if (p14dev->target->child) {
                        p14dev->target->child->color_info = p14dev->saved_target_color_info;

                        set_dev_proc(p14dev->target->child, encode_color, p14dev->saved_target_encode_color);
                        set_dev_proc(p14dev->target->child, decode_color, p14dev->saved_target_decode_color);
                        set_dev_proc(p14dev->target->child, get_color_mapping_procs, p14dev->saved_target_get_color_mapping_procs);
                        set_dev_proc(p14dev->target->child, get_color_comp_index, p14dev->saved_target_get_color_comp_index);
                    }

                    pgs->get_cmap_procs = p14dev->save_get_cmap_procs;
                    gx_set_cmap_procs(pgs, p14dev->target);

                    subclass_device = p14dev->target;
                    p14dev->target = p14dev->target->child;

                    code = dev_proc(dev, create_compositor)(dev, pcdev, pcte, pgs, memory, cdev);

                    p14dev->target = subclass_device;

                    return code;
                }
                break;
            default:
                code = dev_proc(dev, create_compositor)(dev, pcdev, pcte, pgs, memory, cdev);
                break;
        }
    } else {
        code = dev_proc(dev, create_compositor)(dev, pcdev, pcte, pgs, memory, cdev);
    }
    set_dev_proc(dev, create_compositor, gx_subclass_create_compositor);
    return code;
}