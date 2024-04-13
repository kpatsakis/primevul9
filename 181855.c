lyp_data_check_options(struct ly_ctx *ctx, int options, const char *func)
{
    int x = options & LYD_OPT_TYPEMASK;

    /* LYD_OPT_WHENAUTODEL can be used only with LYD_OPT_DATA or LYD_OPT_CONFIG */
    if (options & LYD_OPT_WHENAUTODEL) {
        if ((x == LYD_OPT_EDIT) || (x == LYD_OPT_NOTIF_FILTER)) {
            LOGERR(ctx, LY_EINVAL, "%s: Invalid options 0x%x (LYD_OPT_DATA_WHENAUTODEL can be used only with LYD_OPT_DATA or LYD_OPT_CONFIG)",
                   func, options);
            return 1;
        }
    }

    if (options & (LYD_OPT_DATA_ADD_YANGLIB | LYD_OPT_DATA_NO_YANGLIB)) {
        if (x != LYD_OPT_DATA) {
            LOGERR(ctx, LY_EINVAL, "%s: Invalid options 0x%x (LYD_OPT_DATA_*_YANGLIB can be used only with LYD_OPT_DATA)",
                   func, options);
            return 1;
        }
    }

    /* "is power of 2" algorithm, with 0 exception */
    if (x && !(x && !(x & (x - 1)))) {
        LOGERR(ctx, LY_EINVAL, "%s: Invalid options 0x%x (multiple data type flags set).", func, options);
        return 1;
    }

    return 0;
}