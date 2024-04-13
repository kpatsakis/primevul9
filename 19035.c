dnsc_parse_keys(struct dnsc_env *env, struct config_file *cfg)
{
	struct config_strlist *head;
	size_t cert_id, keypair_id;
	size_t c;
	char *nm;

	env->keypairs_count = 0U;
	for (head = cfg->dnscrypt_secret_key; head; head = head->next) {
		env->keypairs_count++;
	}

	env->keypairs = sodium_allocarray(env->keypairs_count,
		sizeof *env->keypairs);
	env->certs = sodium_allocarray(env->signed_certs_count,
		sizeof *env->certs);

	cert_id = 0U;
	keypair_id = 0U;
	for(head = cfg->dnscrypt_secret_key; head; head = head->next, keypair_id++) {
		char fingerprint[80];
		int found_cert = 0;
		KeyPair *current_keypair = &env->keypairs[keypair_id];
		nm = dnsc_chroot_path(cfg, head->str);
		if(dnsc_read_from_file(
				nm,
				(char *)(current_keypair->crypt_secretkey),
				crypto_box_SECRETKEYBYTES) != 0) {
			fatal_exit("dnsc_parse_keys: failed to load %s: %s", head->str, strerror(errno));
		}
		verbose(VERB_OPS, "Loaded key %s", head->str);
		if (crypto_scalarmult_base(current_keypair->crypt_publickey,
			current_keypair->crypt_secretkey) != 0) {
			fatal_exit("dnsc_parse_keys: could not generate public key from %s", head->str);
		}
		dnsc_key_to_fingerprint(fingerprint, current_keypair->crypt_publickey);
		verbose(VERB_OPS, "Crypt public key fingerprint for %s: %s", head->str, fingerprint);
		// find the cert matching this key
		for(c = 0; c < env->signed_certs_count; c++) {
			if(memcmp(current_keypair->crypt_publickey,
				env->signed_certs[c].server_publickey,
				crypto_box_PUBLICKEYBYTES) == 0) {
				dnsccert *current_cert = &env->certs[cert_id++];
				found_cert = 1;
				current_cert->keypair = current_keypair;
				memcpy(current_cert->magic_query,
				       env->signed_certs[c].magic_query,
					sizeof env->signed_certs[c].magic_query);
				memcpy(current_cert->es_version,
				       env->signed_certs[c].version_major,
				       sizeof env->signed_certs[c].version_major
				);
				dnsc_key_to_fingerprint(fingerprint,
							current_cert->keypair->crypt_publickey);
				verbose(VERB_OPS, "Crypt public key fingerprint for %s: %s",
					head->str, fingerprint);
				verbose(VERB_OPS, "Using %s",
					key_get_es_version(current_cert->es_version));
#ifndef USE_DNSCRYPT_XCHACHA20
				if (current_cert->es_version[1] == 0x02) {
				    fatal_exit("Certificate for XChacha20 but libsodium does not support it.");
				}
#endif

            		}
        	}
		if (!found_cert) {
		    fatal_exit("dnsc_parse_keys: could not match certificate for key "
			       "%s. Unable to determine ES version.",
			       head->str);
		}
	}
	return cert_id;
}