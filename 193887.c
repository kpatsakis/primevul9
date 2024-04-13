static tool_rc tpm2_tool_onstop(ESYS_CONTEXT *ectx) {
    UNUSED(ectx);

    if (!ctx.import_tpm) {
        return tool_rc_success;
    }

    return tpm2_session_close(&ctx.parent.object.session);
}