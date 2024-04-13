EXPORTED void content_md5_hdr(struct transaction_t *txn,
                              const unsigned char *md5)
{
    char base64[MD5_BASE64_LEN+1];

    sasl_encode64((char *) md5, MD5_DIGEST_LENGTH, base64, MD5_BASE64_LEN, NULL);
    simple_hdr(txn, "Content-MD5", base64);
}