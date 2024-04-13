tiff_load_read (thandle_t handle, tdata_t buf, tsize_t size)
{
        TiffContext *context = (TiffContext *)handle;
        
        if (context->pos + size > context->used)
                return 0;
        
        memcpy (buf, context->buffer + context->pos, size);
        context->pos += size;
        return size;
}