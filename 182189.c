int sc_pkcs15_encode_prkdf_entry(sc_context_t *ctx, const struct sc_pkcs15_object *obj,
				 u8 **buf, size_t *buflen)
{
	struct sc_asn1_entry asn1_com_key_attr[C_ASN1_COM_KEY_ATTR_SIZE];
	struct sc_asn1_entry asn1_com_prkey_attr[C_ASN1_COM_PRKEY_ATTR_SIZE];
	struct sc_asn1_entry asn1_rsakey_attr[C_ASN1_RSAKEY_ATTR_SIZE];
	struct sc_asn1_entry asn1_prk_rsa_attr[C_ASN1_PRK_RSA_ATTR_SIZE];
	struct sc_asn1_entry asn1_dsakey_attr[C_ASN1_DSAKEY_ATTR_SIZE];
	struct sc_asn1_entry asn1_prk_dsa_attr[C_ASN1_PRK_DSA_ATTR_SIZE];
	struct sc_asn1_entry asn1_dsakey_value_attr[C_ASN1_DSAKEY_VALUE_ATTR_SIZE];
	struct sc_asn1_entry asn1_dsakey_i_p_attr[C_ASN1_DSAKEY_I_P_ATTR_SIZE];
	struct sc_asn1_entry asn1_gostr3410key_attr[C_ASN1_GOSTR3410KEY_ATTR_SIZE];
	struct sc_asn1_entry asn1_prk_gostr3410_attr[C_ASN1_PRK_GOSTR3410_ATTR_SIZE];
	struct sc_asn1_entry asn1_ecckey_attr[C_ASN1_ECCKEY_ATTR];
	struct sc_asn1_entry asn1_prk_ecc_attr[C_ASN1_PRK_ECC_ATTR];
	struct sc_asn1_entry asn1_prkey[C_ASN1_PRKEY_SIZE];
	struct sc_asn1_entry asn1_supported_algorithms[C_ASN1_SUPPORTED_ALGORITHMS_SIZE];
	struct sc_asn1_pkcs15_object rsa_prkey_obj = {
		(struct sc_pkcs15_object *) obj, asn1_com_key_attr,
		asn1_com_prkey_attr, asn1_prk_rsa_attr
	};
	struct sc_asn1_pkcs15_object dsa_prkey_obj = {
		(struct sc_pkcs15_object *) obj, asn1_com_key_attr,
		asn1_com_prkey_attr, asn1_prk_dsa_attr
	};
	struct sc_asn1_pkcs15_object gostr3410_prkey_obj = {
		(struct sc_pkcs15_object *) obj,
		asn1_com_key_attr, asn1_com_prkey_attr,
		asn1_prk_gostr3410_attr
	};
	struct sc_asn1_pkcs15_object ecc_prkey_obj = {
		(struct sc_pkcs15_object *) obj,
		asn1_com_key_attr, asn1_com_prkey_attr,
		asn1_prk_ecc_attr
	};
	struct sc_pkcs15_prkey_info *prkey = (struct sc_pkcs15_prkey_info *) obj->data;
	struct sc_pkcs15_keyinfo_gostparams *keyinfo_gostparams;
	int r, i;
	size_t af_len, usage_len;

	sc_copy_asn1_entry(c_asn1_prkey, asn1_prkey);
	sc_copy_asn1_entry(c_asn1_supported_algorithms, asn1_supported_algorithms);

	sc_copy_asn1_entry(c_asn1_prk_rsa_attr, asn1_prk_rsa_attr);
	sc_copy_asn1_entry(c_asn1_rsakey_attr, asn1_rsakey_attr);
	sc_copy_asn1_entry(c_asn1_prk_dsa_attr, asn1_prk_dsa_attr);
	sc_copy_asn1_entry(c_asn1_dsakey_attr, asn1_dsakey_attr);
	sc_copy_asn1_entry(c_asn1_dsakey_value_attr, asn1_dsakey_value_attr);
	sc_copy_asn1_entry(c_asn1_dsakey_i_p_attr, asn1_dsakey_i_p_attr);
	sc_copy_asn1_entry(c_asn1_prk_gostr3410_attr, asn1_prk_gostr3410_attr);
	sc_copy_asn1_entry(c_asn1_gostr3410key_attr, asn1_gostr3410key_attr);
	sc_copy_asn1_entry(c_asn1_prk_ecc_attr, asn1_prk_ecc_attr);
	sc_copy_asn1_entry(c_asn1_ecckey_attr, asn1_ecckey_attr);

	sc_copy_asn1_entry(c_asn1_com_prkey_attr, asn1_com_prkey_attr);
	sc_copy_asn1_entry(c_asn1_com_key_attr, asn1_com_key_attr);

	switch (obj->type) {
	case SC_PKCS15_TYPE_PRKEY_RSA:
		sc_format_asn1_entry(asn1_prkey + 0, &rsa_prkey_obj, NULL, 1);
		sc_format_asn1_entry(asn1_prk_rsa_attr + 0, asn1_rsakey_attr, NULL, 1);
		sc_format_asn1_entry(asn1_rsakey_attr + 0, &prkey->path, NULL, 1);
		sc_format_asn1_entry(asn1_rsakey_attr + 1, &prkey->modulus_length, NULL, 1);
		break;
	case SC_PKCS15_TYPE_PRKEY_EC:
		sc_format_asn1_entry(asn1_prkey + 1, &ecc_prkey_obj, NULL, 1);
		sc_format_asn1_entry(asn1_prk_ecc_attr + 0, asn1_ecckey_attr, NULL, 1);
		sc_format_asn1_entry(asn1_ecckey_attr + 0, &prkey->path, NULL, 1);
		sc_format_asn1_entry(asn1_ecckey_attr + 1, &prkey->field_length, NULL, 1);
		break;
	case SC_PKCS15_TYPE_PRKEY_DSA:
		sc_format_asn1_entry(asn1_prkey + 2, &dsa_prkey_obj, NULL, 1);
		sc_format_asn1_entry(asn1_prk_dsa_attr + 0, asn1_dsakey_value_attr, NULL, 1);
		if (prkey->path.type != SC_PATH_TYPE_PATH_PROT) {
			/* indirect: just add the path */
			sc_format_asn1_entry(asn1_dsakey_value_attr + 0, &prkey->path, NULL, 1);
		}
		else {
			/* indirect-protected */
			sc_format_asn1_entry(asn1_dsakey_value_attr + 1, asn1_dsakey_i_p_attr, NULL, 1);
			sc_format_asn1_entry(asn1_dsakey_i_p_attr + 0, &prkey->path, NULL, 1);
		}
		break;
	case SC_PKCS15_TYPE_PRKEY_GOSTR3410:
		sc_format_asn1_entry(asn1_prkey + 3, &gostr3410_prkey_obj, NULL, 1);
		sc_format_asn1_entry(asn1_prk_gostr3410_attr + 0, asn1_gostr3410key_attr, NULL, 1);
		sc_format_asn1_entry(asn1_gostr3410key_attr + 0, &prkey->path, NULL, 1);
		if (prkey->params.len == sizeof(*keyinfo_gostparams))   {
			keyinfo_gostparams = prkey->params.data;
			sc_format_asn1_entry(asn1_gostr3410key_attr + 1, &keyinfo_gostparams->gostr3410, NULL, 1);
			sc_format_asn1_entry(asn1_gostr3410key_attr + 2, &keyinfo_gostparams->gostr3411, NULL, 1);
			sc_format_asn1_entry(asn1_gostr3410key_attr + 3, &keyinfo_gostparams->gost28147, NULL, 1);
		}
		break;
	default:
		sc_log(ctx, "Invalid private key type: %X", obj->type);
		LOG_FUNC_RETURN(ctx, SC_ERROR_INTERNAL);
		break;
	}
	sc_format_asn1_entry(asn1_com_key_attr + 0, &prkey->id, NULL, 1);
	usage_len = sizeof(prkey->usage);
	sc_format_asn1_entry(asn1_com_key_attr + 1, &prkey->usage, &usage_len, 1);
	if (prkey->native == 0)
		sc_format_asn1_entry(asn1_com_key_attr + 2, &prkey->native, NULL, 1);
	if (prkey->access_flags) {
		af_len = sizeof(prkey->access_flags);
		sc_format_asn1_entry(asn1_com_key_attr + 3, &prkey->access_flags, &af_len, 1);
	}
	if (prkey->key_reference >= 0)
		sc_format_asn1_entry(asn1_com_key_attr + 4, &prkey->key_reference, NULL, 1);

	for (i=0; i<SC_MAX_SUPPORTED_ALGORITHMS && prkey->algo_refs[i]; i++)   {
		sc_log(ctx, "Encode algorithm(%i) %i", i, prkey->algo_refs[i]);
		sc_format_asn1_entry(asn1_supported_algorithms + i, &prkey->algo_refs[i], NULL, 1);
	}
	sc_format_asn1_entry(asn1_com_key_attr + 5, asn1_supported_algorithms, NULL, prkey->algo_refs[0] != 0);

	if (prkey->subject.value && prkey->subject.len)
		sc_format_asn1_entry(asn1_com_prkey_attr + 0, prkey->subject.value, &prkey->subject.len, 1);
	else
		memset(asn1_com_prkey_attr, 0, sizeof(asn1_com_prkey_attr));

	r = sc_asn1_encode(ctx, asn1_prkey, buf, buflen);

	sc_log(ctx, "Key path %s", sc_print_path(&prkey->path));
	return r;
}