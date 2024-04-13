size_t olm_pk_mac_length(
    OlmPkEncryption *encryption
) {
    return olm::encode_base64_length(_olm_cipher_aes_sha_256_ops.mac_length(olm_pk_cipher));
}