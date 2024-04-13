static char *_pam_compose_pwd_restriction_string(struct pwb_context *ctx,
						 struct wbcUserPasswordPolicyInfo *i)
{
	char *str = NULL;

	if (!i) {
		goto failed;
	}

	str = talloc_asprintf(ctx, _("Your password "));
	if (!str) {
		goto failed;
	}

	if (i->min_length_password > 0) {
		str = talloc_asprintf_append(str,
			       _("must be at least %d characters; "),
			       i->min_length_password);
		if (!str) {
			goto failed;
		}
	}

	if (i->password_history > 0) {
		str = talloc_asprintf_append(str,
			       _("cannot repeat any of your previous %d "
			        "passwords; "),
			       i->password_history);
		if (!str) {
			goto failed;
		}
	}

	if (i->password_properties & WBC_DOMAIN_PASSWORD_COMPLEX) {
		str = talloc_asprintf_append(str,
			       _("must contain capitals, numerals "
			         "or punctuation; "
			         "and cannot contain your account "
			         "or full name; "));
		if (!str) {
			goto failed;
		}
	}

	str = talloc_asprintf_append(str,
		       _("Please type a different password. "
		         "Type a password which meets these requirements in "
		         "both text boxes."));
	if (!str) {
		goto failed;
	}

	return str;

 failed:
	TALLOC_FREE(str);
	return NULL;
}