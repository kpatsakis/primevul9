static tool_rc tpm_import(ESYS_CONTEXT *ectx) {

    TPM2B_DATA enc_key = TPM2B_EMPTY_INIT;
    TPM2B_PUBLIC public = TPM2B_EMPTY_INIT;
    TPM2B_PRIVATE duplicate;
    TPM2B_ENCRYPTED_SECRET encrypted_seed;
    TPM2B_PRIVATE *imported_private = NULL;
    TPMT_SYM_DEF_OBJECT sym_alg;

    tool_rc rc;
    bool result = set_key_algorithm(ctx.key_type, &sym_alg);
    if (!result) {
        return tool_rc_general_error;
    }

    /* Symmetric key */
    if (ctx.input_enc_key_file) {
        enc_key.size = 16;
        result = files_load_bytes_from_path(ctx.input_enc_key_file,
                enc_key.buffer, &enc_key.size);
        if (!result) {
            LOG_ERR("Failed to load symmetric encryption key\"%s\"",
                    ctx.input_enc_key_file);
            return tool_rc_general_error;
        }
        if (enc_key.size != 16) {
            LOG_ERR("Invalid AES key size, got %u bytes, expected 16",
                    enc_key.size);
            return tool_rc_general_error;
        }
    }

    /* Private key */
    result = files_load_private(ctx.input_key_file, &duplicate);
    if (!result) {
        LOG_ERR("Failed to load duplicate \"%s\"", ctx.input_key_file);
        return tool_rc_general_error;
    }

    /* Encrypted seed */
    result = files_load_encrypted_seed(ctx.input_seed_file, &encrypted_seed);
    if (!result) {
        LOG_ERR("Failed to load encrypted seed \"%s\"", ctx.input_seed_file);
        return tool_rc_general_error;
    }

    /* Public key */
    result = files_load_public(ctx.public_key_file, &public);
    if (!result) {
        LOG_ERR(":( Failed to load public key \"%s\"", ctx.public_key_file);
        return tool_rc_general_error;
    }

    if (ctx.policy) {
        public.publicArea.authPolicy.size =
            sizeof(public.publicArea.authPolicy.buffer);
        result = files_load_bytes_from_path(ctx.policy,
        public.publicArea.authPolicy.buffer,
        &public.publicArea.authPolicy.size);
        if (!result) {
            LOG_ERR("Failed to copy over the auth policy to the public data");
            return tool_rc_general_error;
        }
    }

    if (!ctx.cp_hash_path) {
        rc = tpm2_import(ectx, &ctx.parent.object, &enc_key, &public, &duplicate,
            &encrypted_seed, &sym_alg, &imported_private, NULL);
        if (rc != tool_rc_success) {
            return rc;
        }

        assert(imported_private);

        result = files_save_private(imported_private, ctx.private_key_file);
        free(imported_private);
        if (!result) {
            LOG_ERR("Failed to save private key into file \"%s\"",
                    ctx.private_key_file);
            return tool_rc_general_error;
        }
        return tool_rc_success;
    }

    TPM2B_DIGEST cp_hash = { .size = 0 };
    rc = tpm2_import(ectx, &ctx.parent.object, &enc_key, &public, &duplicate,
            &encrypted_seed, &sym_alg, &imported_private, &cp_hash);
    if (rc != tool_rc_success) {
        return rc;
    }

    result = files_save_digest(&cp_hash, ctx.cp_hash_path);
    if (!result) {
        rc = tool_rc_general_error;
    }
    return rc;

}