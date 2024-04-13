check_host_key(char *hostname, struct sockaddr *hostaddr, u_short port,
    Key *host_key, int readonly,
    char **user_hostfiles, u_int num_user_hostfiles,
    char **system_hostfiles, u_int num_system_hostfiles)
{
	HostStatus host_status;
	HostStatus ip_status;
	Key *raw_key = NULL;
	char *ip = NULL, *host = NULL;
	char hostline[1000], *hostp, *fp, *ra;
	char msg[1024];
	const char *type;
	const struct hostkey_entry *host_found, *ip_found;
	int len, cancelled_forwarding = 0;
	int local = sockaddr_is_local(hostaddr);
	int r, want_cert = key_is_cert(host_key), host_ip_differ = 0;
	struct hostkeys *host_hostkeys, *ip_hostkeys;
	u_int i;

	/*
	 * Force accepting of the host key for loopback/localhost. The
	 * problem is that if the home directory is NFS-mounted to multiple
	 * machines, localhost will refer to a different machine in each of
	 * them, and the user will get bogus HOST_CHANGED warnings.  This
	 * essentially disables host authentication for localhost; however,
	 * this is probably not a real problem.
	 */
	if (options.no_host_authentication_for_localhost == 1 && local &&
	    options.host_key_alias == NULL) {
		debug("Forcing accepting of host key for "
		    "loopback/localhost.");
		return 0;
	}

	/*
	 * Prepare the hostname and address strings used for hostkey lookup.
	 * In some cases, these will have a port number appended.
	 */
	get_hostfile_hostname_ipaddr(hostname, hostaddr, port, &host, &ip);

	/*
	 * Turn off check_host_ip if the connection is to localhost, via proxy
	 * command or if we don't have a hostname to compare with
	 */
	if (options.check_host_ip && (local ||
	    strcmp(hostname, ip) == 0 || options.proxy_command != NULL))
		options.check_host_ip = 0;

	host_hostkeys = init_hostkeys();
	for (i = 0; i < num_user_hostfiles; i++)
		load_hostkeys(host_hostkeys, host, user_hostfiles[i]);
	for (i = 0; i < num_system_hostfiles; i++)
		load_hostkeys(host_hostkeys, host, system_hostfiles[i]);

	ip_hostkeys = NULL;
	if (!want_cert && options.check_host_ip) {
		ip_hostkeys = init_hostkeys();
		for (i = 0; i < num_user_hostfiles; i++)
			load_hostkeys(ip_hostkeys, ip, user_hostfiles[i]);
		for (i = 0; i < num_system_hostfiles; i++)
			load_hostkeys(ip_hostkeys, ip, system_hostfiles[i]);
	}

 retry:
	/* Reload these as they may have changed on cert->key downgrade */
	want_cert = key_is_cert(host_key);
	type = key_type(host_key);

	/*
	 * Check if the host key is present in the user's list of known
	 * hosts or in the systemwide list.
	 */
	host_status = check_key_in_hostkeys(host_hostkeys, host_key,
	    &host_found);

	/*
	 * Also perform check for the ip address, skip the check if we are
	 * localhost, looking for a certificate, or the hostname was an ip
	 * address to begin with.
	 */
	if (!want_cert && ip_hostkeys != NULL) {
		ip_status = check_key_in_hostkeys(ip_hostkeys, host_key,
		    &ip_found);
		if (host_status == HOST_CHANGED &&
		    (ip_status != HOST_CHANGED || 
		    (ip_found != NULL &&
		    !key_equal(ip_found->key, host_found->key))))
			host_ip_differ = 1;
	} else
		ip_status = host_status;

	switch (host_status) {
	case HOST_OK:
		/* The host is known and the key matches. */
		debug("Host '%.200s' is known and matches the %s host %s.",
		    host, type, want_cert ? "certificate" : "key");
		debug("Found %s in %s:%lu", want_cert ? "CA key" : "key",
		    host_found->file, host_found->line);
		if (want_cert && !check_host_cert(hostname, host_key))
			goto fail;
		if (options.check_host_ip && ip_status == HOST_NEW) {
			if (readonly || want_cert)
				logit("%s host key for IP address "
				    "'%.128s' not in list of known hosts.",
				    type, ip);
			else if (!add_host_to_hostfile(user_hostfiles[0], ip,
			    host_key, options.hash_known_hosts))
				logit("Failed to add the %s host key for IP "
				    "address '%.128s' to the list of known "
				    "hosts (%.30s).", type, ip,
				    user_hostfiles[0]);
			else
				logit("Warning: Permanently added the %s host "
				    "key for IP address '%.128s' to the list "
				    "of known hosts.", type, ip);
		} else if (options.visual_host_key) {
			fp = key_fingerprint(host_key, SSH_FP_MD5, SSH_FP_HEX);
			ra = key_fingerprint(host_key, SSH_FP_MD5,
			    SSH_FP_RANDOMART);
			logit("Host key fingerprint is %s\n%s\n", fp, ra);
			free(ra);
			free(fp);
		}
		break;
	case HOST_NEW:
		if (options.host_key_alias == NULL && port != 0 &&
		    port != SSH_DEFAULT_PORT) {
			debug("checking without port identifier");
			if (check_host_key(hostname, hostaddr, 0, host_key,
			    ROQUIET, user_hostfiles, num_user_hostfiles,
			    system_hostfiles, num_system_hostfiles) == 0) {
				debug("found matching key w/out port");
				break;
			}
		}
		if (readonly || want_cert)
			goto fail;
		/* The host is new. */
		if (options.strict_host_key_checking == 1) {
			/*
			 * User has requested strict host key checking.  We
			 * will not add the host key automatically.  The only
			 * alternative left is to abort.
			 */
			error("No %s host key is known for %.200s and you "
			    "have requested strict checking.", type, host);
			goto fail;
		} else if (options.strict_host_key_checking == 2) {
			char msg1[1024], msg2[1024];

			if (show_other_keys(host_hostkeys, host_key))
				snprintf(msg1, sizeof(msg1),
				    "\nbut keys of different type are already"
				    " known for this host.");
			else
				snprintf(msg1, sizeof(msg1), ".");
			/* The default */
			fp = key_fingerprint(host_key, SSH_FP_MD5, SSH_FP_HEX);
			ra = key_fingerprint(host_key, SSH_FP_MD5,
			    SSH_FP_RANDOMART);
			msg2[0] = '\0';
			if (options.verify_host_key_dns) {
				if (matching_host_key_dns)
					snprintf(msg2, sizeof(msg2),
					    "Matching host key fingerprint"
					    " found in DNS.\n");
				else
					snprintf(msg2, sizeof(msg2),
					    "No matching host key fingerprint"
					    " found in DNS.\n");
			}
			snprintf(msg, sizeof(msg),
			    "The authenticity of host '%.200s (%s)' can't be "
			    "established%s\n"
			    "%s key fingerprint is %s.%s%s\n%s"
			    "Are you sure you want to continue connecting "
			    "(yes/no)? ",
			    host, ip, msg1, type, fp,
			    options.visual_host_key ? "\n" : "",
			    options.visual_host_key ? ra : "",
			    msg2);
			free(ra);
			free(fp);
			if (!confirm(msg))
				goto fail;
		}
		/*
		 * If not in strict mode, add the key automatically to the
		 * local known_hosts file.
		 */
		if (options.check_host_ip && ip_status == HOST_NEW) {
			snprintf(hostline, sizeof(hostline), "%s,%s", host, ip);
			hostp = hostline;
			if (options.hash_known_hosts) {
				/* Add hash of host and IP separately */
				r = add_host_to_hostfile(user_hostfiles[0],
				    host, host_key, options.hash_known_hosts) &&
				    add_host_to_hostfile(user_hostfiles[0], ip,
				    host_key, options.hash_known_hosts);
			} else {
				/* Add unhashed "host,ip" */
				r = add_host_to_hostfile(user_hostfiles[0],
				    hostline, host_key,
				    options.hash_known_hosts);
			}
		} else {
			r = add_host_to_hostfile(user_hostfiles[0], host,
			    host_key, options.hash_known_hosts);
			hostp = host;
		}

		if (!r)
			logit("Failed to add the host to the list of known "
			    "hosts (%.500s).", user_hostfiles[0]);
		else
			logit("Warning: Permanently added '%.200s' (%s) to the "
			    "list of known hosts.", hostp, type);
		break;
	case HOST_REVOKED:
		error("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
		error("@       WARNING: REVOKED HOST KEY DETECTED!               @");
		error("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
		error("The %s host key for %s is marked as revoked.", type, host);
		error("This could mean that a stolen key is being used to");
		error("impersonate this host.");

		/*
		 * If strict host key checking is in use, the user will have
		 * to edit the key manually and we can only abort.
		 */
		if (options.strict_host_key_checking) {
			error("%s host key for %.200s was revoked and you have "
			    "requested strict checking.", type, host);
			goto fail;
		}
		goto continue_unsafe;

	case HOST_CHANGED:
		if (want_cert) {
			/*
			 * This is only a debug() since it is valid to have
			 * CAs with wildcard DNS matches that don't match
			 * all hosts that one might visit.
			 */
			debug("Host certificate authority does not "
			    "match %s in %s:%lu", CA_MARKER,
			    host_found->file, host_found->line);
			goto fail;
		}
		if (readonly == ROQUIET)
			goto fail;
		if (options.check_host_ip && host_ip_differ) {
			char *key_msg;
			if (ip_status == HOST_NEW)
				key_msg = "is unknown";
			else if (ip_status == HOST_OK)
				key_msg = "is unchanged";
			else
				key_msg = "has a different value";
			error("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
			error("@       WARNING: POSSIBLE DNS SPOOFING DETECTED!          @");
			error("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
			error("The %s host key for %s has changed,", type, host);
			error("and the key for the corresponding IP address %s", ip);
			error("%s. This could either mean that", key_msg);
			error("DNS SPOOFING is happening or the IP address for the host");
			error("and its host key have changed at the same time.");
			if (ip_status != HOST_NEW)
				error("Offending key for IP in %s:%lu",
				    ip_found->file, ip_found->line);
		}
		/* The host key has changed. */
		warn_changed_key(host_key);
		error("Add correct host key in %.100s to get rid of this message.",
		    user_hostfiles[0]);
		error("Offending %s key in %s:%lu", key_type(host_found->key),
		    host_found->file, host_found->line);

		/*
		 * If strict host key checking is in use, the user will have
		 * to edit the key manually and we can only abort.
		 */
		if (options.strict_host_key_checking) {
			error("%s host key for %.200s has changed and you have "
			    "requested strict checking.", type, host);
			goto fail;
		}

 continue_unsafe:
		/*
		 * If strict host key checking has not been requested, allow
		 * the connection but without MITM-able authentication or
		 * forwarding.
		 */
		if (options.password_authentication) {
			error("Password authentication is disabled to avoid "
			    "man-in-the-middle attacks.");
			options.password_authentication = 0;
			cancelled_forwarding = 1;
		}
		if (options.kbd_interactive_authentication) {
			error("Keyboard-interactive authentication is disabled"
			    " to avoid man-in-the-middle attacks.");
			options.kbd_interactive_authentication = 0;
			options.challenge_response_authentication = 0;
			cancelled_forwarding = 1;
		}
		if (options.challenge_response_authentication) {
			error("Challenge/response authentication is disabled"
			    " to avoid man-in-the-middle attacks.");
			options.challenge_response_authentication = 0;
			cancelled_forwarding = 1;
		}
		if (options.forward_agent) {
			error("Agent forwarding is disabled to avoid "
			    "man-in-the-middle attacks.");
			options.forward_agent = 0;
			cancelled_forwarding = 1;
		}
		if (options.forward_x11) {
			error("X11 forwarding is disabled to avoid "
			    "man-in-the-middle attacks.");
			options.forward_x11 = 0;
			cancelled_forwarding = 1;
		}
		if (options.num_local_forwards > 0 ||
		    options.num_remote_forwards > 0) {
			error("Port forwarding is disabled to avoid "
			    "man-in-the-middle attacks.");
			options.num_local_forwards =
			    options.num_remote_forwards = 0;
			cancelled_forwarding = 1;
		}
		if (options.tun_open != SSH_TUNMODE_NO) {
			error("Tunnel forwarding is disabled to avoid "
			    "man-in-the-middle attacks.");
			options.tun_open = SSH_TUNMODE_NO;
			cancelled_forwarding = 1;
		}
		if (options.exit_on_forward_failure && cancelled_forwarding)
			fatal("Error: forwarding disabled due to host key "
			    "check failure");
		
		/*
		 * XXX Should permit the user to change to use the new id.
		 * This could be done by converting the host key to an
		 * identifying sentence, tell that the host identifies itself
		 * by that sentence, and ask the user if he/she wishes to
		 * accept the authentication.
		 */
		break;
	case HOST_FOUND:
		fatal("internal error");
		break;
	}

	if (options.check_host_ip && host_status != HOST_CHANGED &&
	    ip_status == HOST_CHANGED) {
		snprintf(msg, sizeof(msg),
		    "Warning: the %s host key for '%.200s' "
		    "differs from the key for the IP address '%.128s'"
		    "\nOffending key for IP in %s:%lu",
		    type, host, ip, ip_found->file, ip_found->line);
		if (host_status == HOST_OK) {
			len = strlen(msg);
			snprintf(msg + len, sizeof(msg) - len,
			    "\nMatching host key in %s:%lu",
			    host_found->file, host_found->line);
		}
		if (options.strict_host_key_checking == 1) {
			logit("%s", msg);
			error("Exiting, you have requested strict checking.");
			goto fail;
		} else if (options.strict_host_key_checking == 2) {
			strlcat(msg, "\nAre you sure you want "
			    "to continue connecting (yes/no)? ", sizeof(msg));
			if (!confirm(msg))
				goto fail;
		} else {
			logit("%s", msg);
		}
	}

	free(ip);
	free(host);
	if (host_hostkeys != NULL)
		free_hostkeys(host_hostkeys);
	if (ip_hostkeys != NULL)
		free_hostkeys(ip_hostkeys);
	return 0;

fail:
	if (want_cert && host_status != HOST_REVOKED) {
		/*
		 * No matching certificate. Downgrade cert to raw key and
		 * search normally.
		 */
		debug("No matching CA found. Retry with plain key");
		raw_key = key_from_private(host_key);
		if (key_drop_cert(raw_key) != 0)
			fatal("Couldn't drop certificate");
		host_key = raw_key;
		goto retry;
	}
	if (raw_key != NULL)
		key_free(raw_key);
	free(ip);
	free(host);
	if (host_hostkeys != NULL)
		free_hostkeys(host_hostkeys);
	if (ip_hostkeys != NULL)
		free_hostkeys(ip_hostkeys);
	return -1;
}