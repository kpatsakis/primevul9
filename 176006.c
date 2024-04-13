tiff_load_map_file (thandle_t handle, tdata_t *buf, toff_t *size)
{
        TiffContext *context = (TiffContext *)handle;
        
        *buf = context->buffer;
        *size = context->used;
        
        return 0;
}