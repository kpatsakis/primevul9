SECU_GetModulePassword(PK11SlotInfo *slot, PRBool retry, void *arg)
{
	char *prompt = NULL;
	cms_context *cms = (cms_context *)arg;
	secuPWData *pwdata;
	secuPWData pwxtrn = { .source = PW_DEVICE, .orig_source = PW_DEVICE, .data = NULL };
	char *pw;
	int rc;

	ingress();

	if (PK11_ProtectedAuthenticationPath(slot)) {
		dprintf("prompting for PW_DEVICE data");
		pwdata = &pwxtrn;
	} else {
		dprintf("using pwdata from cms");
		pwdata = &cms->pwdata;
	}

	if (pwdata->source <= PW_SOURCE_INVALID ||
	    pwdata->source >= PW_SOURCE_MAX ||
	    pwdata->orig_source <= PW_SOURCE_INVALID ||
	    pwdata->orig_source >= PW_SOURCE_MAX) {
		dprintf("pwdata is invalid");
		return NULL;
	}

	dprintf("pwdata:%p retry:%d", pwdata, retry);
	dprintf("pwdata->source:%s (%d) orig:%s (%d)",
		pw_source_names[pwdata->source], pwdata->source,
		pw_source_names[pwdata->orig_source], pwdata->orig_source);
	dprintf("pwdata->data:%p (\"%s\")", pwdata->data,
		pwdata->data ? pwdata->data : "(null)");

	if (retry) {
		warnx("Incorrect password/PIN entered.");
		if (!can_prompt_again(pwdata)) {
			egress();
			return NULL;
		}
	}

	switch (pwdata->source) {
	case PW_PROMPT:
		rc = asprintf(&prompt, "Enter Password or Pin for \"%s\":",
			      PK11_GetTokenName(slot));
		if (rc < 0)
			return NULL;
		pw = SECU_GetPasswordString(NULL, prompt);
		if (!pw)
			return NULL;
		free(prompt);

		pwdata->source = PW_PLAINTEXT;
		egress();
		return pw;

	case PW_DEVICE:
		dprintf("pwdata->source:PW_DEVICE");
		rc = asprintf(&prompt,
			      "Press Enter, then enter PIN for \"%s\" on external device.\n",
			      PK11_GetTokenName(slot));
		if (rc < 0)
			return NULL;
		pw = SECU_GetPasswordString(NULL, prompt);
		free(prompt);
		return pw;

	case PW_FROMFILEDB:
	case PW_DATABASE:
		dprintf("pwdata->source:%s", pw_source_names[pwdata->source]);
		/* Instead of opening and closing the file every time, get the pw
		 * once, then keep it in memory (duh).
		 */
		pw = SECU_FilePasswd(slot, retry, cms);
		if (pw != NULL) {
			pwdata->source = PW_PLAINTEXT;
			pwdata->data = strdup(pw);
		}
		/* it's already been dup'ed */
		egress();
		return pw;

	case PW_FROMENV:
		dprintf("pwdata->source:PW_FROMENV");
		if (!pwdata || !pwdata->data)
			break;
		pw = get_env(pwdata->data);
		dprintf("env:%s pw:%s", pwdata->data, pw ? pw : "(null)");
		pwdata->data = pw;
		pwdata->source = PW_PLAINTEXT;
		goto PW_PLAINTEXT;

	PW_PLAINTEXT:
	case PW_PLAINTEXT:
		egress();
		if (pwdata && pwdata->data)
			return strdup(pwdata->data);
		return NULL;

	default:
		break;
	}

	warnx("Password check failed: No password found.");
	egress();
	return NULL;
}