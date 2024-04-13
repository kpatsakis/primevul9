free_save_context (TiffSaveContext *context)
{
        g_free (context->buffer);
        g_free (context);
}