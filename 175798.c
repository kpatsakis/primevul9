int epo_finish_copydevice(gx_device *dev, const gx_device *from_dev)
{
    /* We musn't allow the following pointers to remain shared with the from_dev
       because we're about to tell the caller it's only allowed to copy the prototype
       and free the attempted copy of a non-prototype. If from_dev is the prototype
       these pointers won't be set, anyway.
     */
    dev->child = NULL;
    dev->parent = NULL;
    dev->subclass_data = NULL;
    /* Only allow copying the prototype. */
    return (from_dev->memory ? gs_note_error(gs_error_rangecheck) : 0);
}