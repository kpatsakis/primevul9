tiff_load_seek (thandle_t handle, toff_t offset, int whence)
{
        TiffContext *context = (TiffContext *)handle;
        
        switch (whence) {
        case SEEK_SET:
                if (offset > context->used)
                        return -1;
                context->pos = offset;
                break;
        case SEEK_CUR:
                if (offset + context->pos >= context->used)
                        return -1;
                context->pos += offset;
                break;
        case SEEK_END:
                if (offset + context->used > context->used)
                        return -1;
                context->pos = context->used + offset;
                break;
        default:
                return -1;
        }
        return context->pos;
}