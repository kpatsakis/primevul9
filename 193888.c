static tool_rc readpublic(ESYS_CONTEXT *ectx, ESYS_TR handle,
        TPM2B_PUBLIC **public) {

    return tpm2_readpublic(ectx, handle, public, NULL, NULL);
}