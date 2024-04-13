static tool_rc check_options(void) {

    tool_rc rc = tool_rc_success;

    /* Check the tpm import specific options */
    if (ctx.import_tpm) {
        if (!ctx.input_seed_file) {
            LOG_ERR("Expected SymSeed to be specified via \"-s\","
                    " missing option.");
            rc = tool_rc_option_error;
        }

        /* If a key file is specified we choose aes else null
         for symmetricAlgdefinition */
        if (!ctx.input_enc_key_file) {
            ctx.key_type = TPM2_ALG_NULL;
        } else {
            ctx.key_type = TPM2_ALG_AES;
        }

        if (ctx.key_auth_str) {
            LOG_ERR("Cannot specify key password when importing a TPM key.\n"
                "use tpm2_changeauth after import");
            rc = tool_rc_option_error;
        }

    } else { /* Openssl specific option(s) */

        if (!ctx.key_type) {
            LOG_ERR("Expected key type to be specified via \"-G\","
                    " missing option.");
            rc = tool_rc_option_error;
        }

        if (ctx.cp_hash_path) {
            LOG_WARN("CAUTION CpHash calculation includes parameters that"
                     "have a derived/random seed!");
        }
    }

    /* Common options */
    if (!ctx.input_key_file) {
        LOG_ERR("Expected to be imported key data to be specified via \"-i\","
                " missing option.");
        rc = tool_rc_option_error;
    }

    if (!ctx.public_key_file) {
        LOG_ERR("Expected output public file missing, specify \"-u\","
                " missing option.");
        rc = tool_rc_option_error;
    }

    if (!ctx.private_key_file) {
        LOG_ERR("Expected output private file missing, specify \"-r\","
                " missing option.");
        rc = tool_rc_option_error;
    }

    if (!ctx.parent.ctx_path) {
        LOG_ERR("Expected parent key to be specified via \"-C\","
                " missing option.");
        rc = tool_rc_option_error;
    }

    return rc;
}