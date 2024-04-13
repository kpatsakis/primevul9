static bool create_import_key_private_data(TPM2B_PRIVATE *private,
        TPMI_ALG_HASH parent_name_alg,
        TPM2B_MAX_BUFFER *encrypted_duplicate_sensitive,
        TPM2B_DIGEST *outer_hmac) {

    //UINT16 hash_size = tpm2_alg_util_get_hash_size(ctx.name_alg);
    UINT16 parent_hash_size = tpm2_alg_util_get_hash_size(parent_name_alg);

    private->size = sizeof(parent_hash_size) + parent_hash_size
            + encrypted_duplicate_sensitive->size;

    size_t hmac_size_offset = 0;
    TSS2_RC rval = Tss2_MU_UINT16_Marshal(parent_hash_size, private->buffer,
            sizeof(parent_hash_size), &hmac_size_offset);
    if (rval != TPM2_RC_SUCCESS)
    {
        LOG_ERR("Error serializing parent hash size");
        return false;
    }

    memcpy(private->buffer + hmac_size_offset, outer_hmac->buffer,
            parent_hash_size);
    memcpy(private->buffer + hmac_size_offset + parent_hash_size,
            encrypted_duplicate_sensitive->buffer,
            encrypted_duplicate_sensitive->size);

    return true;
}