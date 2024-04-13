gx_default_process_page(gx_device *dev, gx_process_page_options_t *options)
{
    gs_int_rect rect;
    int code = 0;
    void *buffer = NULL;

    /* Possible future improvements in here could be given by us dividing the
     * page up into n chunks, and spawning a thread per chunk to do the
     * process_fn call on. n could be given by NumRenderingThreads. This
     * would give us multi-core advantages even without clist. */
    if (options->init_buffer_fn) {
        code = options->init_buffer_fn(options->arg, dev, dev->memory, dev->width, dev->height, &buffer);
        if (code < 0)
            return code;
    }

    rect.p.x = 0;
    rect.p.y = 0;
    rect.q.x = dev->width;
    rect.q.y = dev->height;
    if (options->process_fn)
        code = options->process_fn(options->arg, dev, dev, &rect, buffer);
    if (code >= 0 && options->output_fn)
        code = options->output_fn(options->arg, dev, buffer);

    if (options->free_buffer_fn)
        options->free_buffer_fn(options->arg, dev, dev->memory, buffer);

    return code;
}