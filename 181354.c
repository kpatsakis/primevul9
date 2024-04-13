static inline bool is_ro_export(FsContext *ctx)
{
    return ctx->export_flags & V9FS_RDONLY;
}