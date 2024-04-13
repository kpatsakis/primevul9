make_available_at_least (TiffContext *context, guint needed)
{
        guchar *new_buffer = NULL;
        guint need_alloc;
        
        need_alloc = context->used + needed;
        if (need_alloc > context->allocated) {
                guint new_size = 1;
                while (new_size < need_alloc)
                        new_size *= 2;

                new_buffer = g_try_realloc (context->buffer, new_size);
                if (new_buffer) {
                        context->buffer = new_buffer;
                        context->allocated = new_size;
                        return TRUE;
                }
                return FALSE;
        }
        return TRUE;
}