long keyctl_reject_key(key_serial_t id, unsigned timeout, unsigned error,
		       key_serial_t ringid)
{
	const struct cred *cred = current_cred();
	struct request_key_auth *rka;
	struct key *instkey, *dest_keyring;
	long ret;

	kenter("%d,%u,%u,%d", id, timeout, error, ringid);

	/* must be a valid error code and mustn't be a kernel special */
	if (error <= 0 ||
	    error >= MAX_ERRNO ||
	    error == ERESTARTSYS ||
	    error == ERESTARTNOINTR ||
	    error == ERESTARTNOHAND ||
	    error == ERESTART_RESTARTBLOCK)
		return -EINVAL;

	/* the appropriate instantiation authorisation key must have been
	 * assumed before calling this */
	ret = -EPERM;
	instkey = cred->request_key_auth;
	if (!instkey)
		goto error;

	rka = instkey->payload.data[0];
	if (rka->target_key->serial != id)
		goto error;

	/* find the destination keyring if present (which must also be
	 * writable) */
	ret = get_instantiation_keyring(ringid, rka, &dest_keyring);
	if (ret < 0)
		goto error;

	/* instantiate the key and link it into a keyring */
	ret = key_reject_and_link(rka->target_key, timeout, error,
				  dest_keyring, instkey);

	key_put(dest_keyring);

	/* discard the assumed authority if it's just been disabled by
	 * instantiation of the key */
	if (ret == 0)
		keyctl_change_reqkey_auth(NULL);

error:
	return ret;
}