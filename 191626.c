_zip_get_encryption_implementation(zip_uint16_t em)
{
    switch (em) {
    case ZIP_EM_TRAD_PKWARE:
	return zip_source_pkware;

    case ZIP_EM_AES_128:
    case ZIP_EM_AES_192:
    case ZIP_EM_AES_256:
	return zip_source_winzip_aes;

    default:
	return NULL;
    }
}