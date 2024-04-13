rc_pdf14_maskbuf_free(gs_memory_t * mem, void *ptr_in, client_name_t cname)
{
    /* Ending the mask buffer. */
    pdf14_rcmask_t *rcmask = (pdf14_rcmask_t * ) ptr_in;
    /* free the pdf14 buffer. */
    if ( rcmask->mask_buf != NULL ){
        pdf14_buf_free(rcmask->mask_buf, mem);
    }
    gs_free_object(mem, rcmask, "rc_pdf14_maskbuf_free");
}