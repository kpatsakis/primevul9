tiff_save_seek (thandle_t handle, toff_t offset, int whence)
{
        TiffSaveContext *context = (TiffSaveContext *)handle;

        switch (whence) {
        case SEEK_SET:
                context->pos = offset;
                break;
        case SEEK_CUR:
                context->pos += offset;
                break;
        case SEEK_END:
                context->pos = context->used + offset;
                break;
        default:
                return -1;
        }
        return context->pos;
}