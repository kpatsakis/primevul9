void doi_log_cert_thinking(uint16_t auth,
				enum ike_cert_type certtype,
				enum certpolicy policy,
				bool gotcertrequest,
				bool send_cert,
				bool send_chain)
{
	DBG(DBG_CONTROL, {
		DBG_log("thinking about whether to send my certificate:");

		struct esb_buf oan;
		struct esb_buf ictn;

		DBG_log("  I have RSA key: %s cert.type: %s ",
			enum_showb(&oakley_auth_names, auth, &oan),
			enum_showb(&ike_cert_type_names, certtype, &ictn));

		struct esb_buf cptn;

		DBG_log("  sendcert: %s and I did%s get a certificate request ",
			enum_showb(&certpolicy_type_names, policy, &cptn),
			gotcertrequest ? "" : " not");

		DBG_log("  so %ssend cert.", send_cert ? "" : "do not ");

		if (!send_cert) {
			if (auth == OAKLEY_PRESHARED_KEY) {
				DBG_log("I did not send a certificate because digital signatures are not being used. (PSK)");
			} else if (certtype == CERT_NONE) {
				DBG_log("I did not send a certificate because I do not have one.");
			} else if (policy == CERT_SENDIFASKED) {
				DBG_log("I did not send my certificate because I was not asked to.");
			} else {
				DBG_log("INVALID AUTH SETTING: %d", auth);
			}
		}
		if (send_chain)
			DBG_log("Sending one or more authcerts");
	});
}