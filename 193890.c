static tool_rc openssl_import(ESYS_CONTEXT *ectx) {

    /*
     * Load the parent public file, or read it from the TPM if not specified.
     * We need this information for encrypting the protection seed.
     */
    bool free_ppub = false;
    tool_rc tmp_rc;
    tool_rc rc = tool_rc_general_error;
    TPM2B_PUBLIC ppub = TPM2B_EMPTY_INIT;
    TPM2B_PUBLIC *parent_pub = NULL;

    bool result;
    tmp_rc = tool_rc_general_error;
    if (ctx.parent_key_public_file) {
        result = files_load_public(ctx.parent_key_public_file, &ppub);
        parent_pub = &ppub;
    } else {
        tmp_rc = readpublic(ectx, ctx.parent.object.tr_handle, &parent_pub);
        free_ppub = true;
        result = tmp_rc == tool_rc_success;
    }
    if (!result) {
        LOG_ERR("Failed loading parent key public.");
        return tmp_rc;
    }

    TPM2B_SENSITIVE private = TPM2B_EMPTY_INIT;
    TPM2B_PUBLIC public = TPM2B_EMPTY_INIT;
    TPM2B_ENCRYPTED_SECRET encrypted_seed = TPM2B_EMPTY_INIT;

    result = tpm2_openssl_import_keys(
        parent_pub,
        &private,
        &public,
        &encrypted_seed,
        ctx.input_key_file,
        ctx.key_type,
        ctx.auth_key_file,
        ctx.policy,
        ctx.key_auth_str,
        ctx.attrs,
        ctx.name_alg
    );
    if (!result)
        goto out;

    TPM2B_PRIVATE *imported_private = NULL;
    tmp_rc = key_import(ectx, parent_pub, &private, &public, &encrypted_seed,
            &imported_private);
    if (tmp_rc != tool_rc_success || ctx.cp_hash_path) {
        rc = tmp_rc;
        goto keyout;
    }

    /*
     * Save the public and imported_private structure to disk
     */
    result = files_save_public(&public, ctx.public_key_file);
    if (!result) {
        goto keyout;
    }

    result = files_save_private(imported_private, ctx.private_key_file);
    if (!result) {
        goto keyout;
    }

    /*
     * Output the stats on the created object on Success.
     */
    tpm2_util_public_to_yaml(&public, NULL);

    rc = tool_rc_success;

keyout:
    free(imported_private);
out:
    if (free_ppub) {
        free(parent_pub);
    }

    return rc;
}