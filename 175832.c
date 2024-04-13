int gx_device_unsubclass(gx_device *dev)
{
    generic_subclass_data *psubclass_data;
    gx_device *parent, *child;
    gs_memory_struct_type_t *a_std = 0, *b_std = 0;
    int dynamic, ref_count;

    /* This should not happen... */
    if (!dev)
        return 0;

    ref_count = dev->rc.ref_count;
    child = dev->child;
    psubclass_data = (generic_subclass_data *)dev->subclass_data;
    parent = dev->parent;
    dynamic = dev->stype_is_dynamic;

    /* We need to account for the fact that we are removing ourselves from
     * the device chain after a clist device has been pushed, due to a
     * compositor action. Since we patched the clist 'create_compositor'
     * method (and target device) when it was pushed.
     * A point to note; we *don't* want to change the forwarding device's
     * 'target', because when we copy the child up to replace 'this' device
     * we do still want the forwarding device to point here. NB its the *child*
     * device that goes away.
     */
    if (psubclass_data != NULL && psubclass_data->forwarding_dev != NULL && psubclass_data->saved_compositor_method)
        psubclass_data->forwarding_dev->procs.create_compositor = psubclass_data->saved_compositor_method;

    /* If ths device's stype is dynamically allocated, keep a copy of it
     * in case we might need it.
     */
    if (dynamic) {
        a_std = (gs_memory_struct_type_t *)dev->stype;
        if (child)
            *a_std = *child->stype;
    }

    /* If ths device has any private storage, free it now */
    if (psubclass_data)
        gs_free_object(dev->memory->non_gc_memory, psubclass_data, "subclass memory for first-last page");

    /* Copy the child device into ths device's memory */
    if (child) {
        b_std = (gs_memory_struct_type_t *)dev->stype;
        rc_decrement(dev->icc_struct, "unsubclass device");
        rc_increment(child->icc_struct);
        memcpy(dev, child, child->stype->ssize);
        /* Patch back the 'stype' in the memory manager */
        gs_set_object_type(child->memory, dev, b_std);

        dev->stype = b_std;
        /* The reference count of the subclassing device may have been changed
         * (eg graphics states pointing to it) after we subclassed the device. We
         * need to ensure that we do not overwrite this when we copy back the subclassed
         * device.
         */
        dev->rc.ref_count = ref_count;

        /* If we have a chain of devices, make sure the chain beond the device we're unsubclassing
         * doesn't get broken, we needd to detach the lower chain and reattach it at the new
         * highest level
         */
        if (child->child)
            child->child->parent = dev;
        child->parent->child = child->child;
    }

    /* How can we have a subclass device with no child ? Simples; when we hit the end of job
     * restore, the devices are not freed in device chain order. To make sure we don't end up
     * following stale pointers, when a device is freed we remove it from the chain and update
     * any danlging poitners to NULL. When we later free the remaining devices its possible that
     * their child pointer can then be NULL.
     */
    if (child) {
        if (child->icc_struct)
            rc_decrement(child->icc_struct, "gx_unsubclass_device, icc_struct");
        if (child->PageList)
            rc_decrement(child->PageList, "gx_unsubclass_device, PageList");
        /* we cannot afford to free the child device if its stype is not dynamic because
         * we can't 'null' the finalise routine, and we cannot permit the device to be finalised
         * because we have copied it up one level, not discarded it.
         * (this shouldn't happen! Child devices are always created with a dynamic stype)
         * If this ever happens garbage collecton will eventually clean up the memory.
         */
        if (child->stype_is_dynamic) {
            /* Make sure that nothing will tyr to follow the device chain, just security here */
            child->parent = NULL;
            child->child = NULL;
            /* Make certainthe memory will be freed, zap the reference count */
            child->rc.ref_count = 0;
            /* We *don't* want to run the finalize routine. This would free the stype and
             * properly handle the icc_struct and PageList, but for devices with a custom
             * finalize (eg psdcmyk) it might also free memory it had allocated, and we're
             * still pointing at that memory in the parent.
             * The indirection through a variable is just to get rid of const warnings.
             */
            b_std = (gs_memory_struct_type_t *)child->stype;
            b_std->finalize = NULL;
            /* Having patched the stype, we need to make sure the memory manager uses it.
             * It keeps a copy in its own data structure, and would use that copy, which would
             * mean it would call the finalize routine that we just patched out.
             */
            gs_set_object_type(dev->memory->stable_memory, child, b_std);
            /* Now (finally) free the child memory */
            gs_free_object(dev->memory->stable_memory, child, "gx_unsubclass_device(device)");
            /* And the stype for it */
            gs_free_const_object(dev->memory->non_gc_memory, b_std, "gs_device_unsubclass(stype)");
            child = 0;
        }
    }
    if(child)
        child->parent = dev;
    dev->parent = parent;

    /* If this device has a dynamic stype, we wnt to keep using it, but we copied
     * the stype pointer from the child when we copied the rest of the device. So
     * we update the stype pointer with the saved pointer to this device's stype.
     */
    if (dynamic) {
        dev->stype = a_std;
        dev->stype_is_dynamic = 1;
    } else {
        dev->stype_is_dynamic = 0;
    }

    return 0;
}