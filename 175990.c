create_save_context (void)
{
        TiffSaveContext *context;

        context = g_new (TiffSaveContext, 1);
        context->buffer = NULL;
        context->allocated = 0;
        context->used = 0;
        context->pos = 0;

        return context;
}