static tool_rc tpm2_tool_onrun(ESYS_CONTEXT *ectx, tpm2_option_flags flags) {
    UNUSED(flags);

    tool_rc rc = check_options();
    if (rc != tool_rc_success) {
        return rc;
    }

    rc = tpm2_util_object_load_auth(ectx, ctx.parent.ctx_path,
            ctx.parent.auth_str, &ctx.parent.object, false,
            TPM2_HANDLE_ALL_W_NV);
    if (rc != tool_rc_success) {
        LOG_ERR("Invalid parent key authorization");
        return rc;
    }

    return ctx.import_tpm ? tpm_import(ectx) : openssl_import(ectx);
}