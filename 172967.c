pdf14_get_buffer_information(const gx_device * dev,
                             gx_pattern_trans_t *transbuff, gs_memory_t *mem,
                             bool free_device)
{
    const pdf14_device * pdev = (pdf14_device *)dev;
    pdf14_buf *buf;
    gs_int_rect rect;
    int x1,y1,width,height;

    if ( pdev->ctx == NULL){
        return 0;  /* this can occur if the pattern is a clist */
    }
#ifdef DEBUG
    pdf14_debug_mask_stack_state(pdev->ctx);
#endif
    buf = pdev->ctx->stack;
    rect = buf->rect;
    transbuff->dirty = &buf->dirty;
    x1 = min(pdev->width, rect.q.x);
    y1 = min(pdev->height, rect.q.y);
    width = x1 - rect.p.x;
    height = y1 - rect.p.y;
    if (width <= 0 || height <= 0 || buf->data == NULL)
        return 0;
    transbuff->n_chan    = buf->n_chan;
    transbuff->has_shape = buf->has_shape;
    transbuff->width     = buf->rect.q.x - buf->rect.p.x;
    transbuff->height    = buf->rect.q.y - buf->rect.p.y;

    if (free_device) {
        transbuff->pdev14 = NULL;
        transbuff->rect = rect;
        if ((width < transbuff->width) || (height < transbuff->height)) {
            /* If the bbox is smaller than the whole buffer than go ahead and
               create a new one to use.  This can occur if we drew in a smaller
               area than was specified by the transparency group rect. */
            int rowstride = (width + 3) & -4;
            int planestride = rowstride * height;
            int k, j;
            byte *buff_ptr_src, *buff_ptr_des;

            transbuff->planestride = planestride;
            transbuff->rowstride = rowstride;
            transbuff->transbytes = gs_alloc_bytes(mem, planestride*buf->n_chan,
                                                   "pdf14_get_buffer_information");
            transbuff->mem = mem;
            for (j = 0; j < transbuff->n_chan; j++) {
                buff_ptr_src = buf->data + j * buf->planestride +
                           buf->rowstride * rect.p.y + rect.p.x;
                buff_ptr_des = transbuff->transbytes + j * planestride;
                for (k = 0; k < height; k++) {
                    memcpy(buff_ptr_des, buff_ptr_src,rowstride);
                    buff_ptr_des += rowstride;
                    buff_ptr_src += buf->rowstride;
                }
            }

        } else {
            /* The entire buffer is used.  Go ahead and grab the pointer and
               clear the pointer in the pdf14 device data buffer so it is not
               freed when we close the device */
            transbuff->planestride = buf->planestride;
            transbuff->rowstride = buf->rowstride;
            transbuff->transbytes = buf->data;
            transbuff->mem = dev->memory;
            buf->data = NULL;  /* So that the buffer is not freed */
        }
        /* Go ahead and free up the pdf14 device */
        dev_proc(dev, close_device)((gx_device *)dev);
#if RAW_DUMP
        /* Dump the buffer that should be going into the pattern */;
        dump_raw_buffer(height, width, transbuff->n_chan,
                    transbuff->planestride, transbuff->rowstride,
                    "pdf14_pattern_buff", transbuff->transbytes);
        global_index++;
#endif
    } else {
        /* Here we are coming from one of the fill image / pattern / mask
           operations */
        transbuff->pdev14 = dev;
        transbuff->planestride = buf->planestride;
        transbuff->rowstride = buf->rowstride;
        transbuff->transbytes = buf->data;
        transbuff->mem = dev->memory;
        transbuff->rect = rect;
#if RAW_DUMP
    /* Dump the buffer that should be going into the pattern */;
        dump_raw_buffer(height, width, buf->n_chan,
                    pdev->ctx->stack->planestride, pdev->ctx->stack->rowstride,
                    "pdf14_pattern_buff", buf->data +
                    transbuff->rowstride * transbuff->rect.p.y +
                    transbuff->rect.p.x);
        global_index++;
#endif
    }
    return(0);
}