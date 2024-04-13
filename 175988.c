tiff_load_size (thandle_t handle)
{
        TiffContext *context = (TiffContext *)handle;
        
        return context->used;
}