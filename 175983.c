tiff_save_write (thandle_t handle, tdata_t buf, tsize_t size)
{
        TiffSaveContext *context = (TiffSaveContext *)handle;

        /* Modify buffer length */
        if (context->pos + size > context->used)
                context->used = context->pos + size;

        /* Realloc */
        if (context->used > context->allocated) {
                context->buffer = g_realloc (context->buffer, context->pos + size);
                context->allocated = context->used;
        }

        /* Now copy the data */
        memcpy (context->buffer + context->pos, buf, size);

        /* Update pos */
        context->pos += size;

        return size;
}