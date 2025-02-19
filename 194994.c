sc_oberthur_read_file(struct sc_pkcs15_card *p15card, const char *in_path,
		unsigned char **out, size_t *out_len,
		int verify_pin)
{
	struct sc_context *ctx = p15card->card->ctx;
	struct sc_card *card = p15card->card;
	struct sc_file *file = NULL;
	struct sc_path path;
	size_t sz;
	int rv;

	LOG_FUNC_CALLED(ctx);
	if (!in_path || !out || !out_len)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_ARGUMENTS, "Cannot read oberthur file");

	sc_log(ctx, "read file '%s'; verify_pin:%i", in_path, verify_pin);

	*out = NULL;
	*out_len = 0;

	sc_format_path(in_path, &path);
	rv = sc_select_file(card, &path, &file);
	if (rv != SC_SUCCESS) {
		sc_file_free(file);
		LOG_TEST_RET(ctx, rv, "Cannot select oberthur file to read");
	}

	if (file->ef_structure == SC_FILE_EF_TRANSPARENT)
		sz = file->size;
	else
		sz = (file->record_length + 2) * file->record_count;

	*out = calloc(sz, 1);
	if (*out == NULL) {
		sc_file_free(file);
		LOG_TEST_RET(ctx, SC_ERROR_OUT_OF_MEMORY, "Cannot read oberthur file");
	}

	if (file->ef_structure == SC_FILE_EF_TRANSPARENT)   {
		rv = sc_read_binary(card, 0, *out, sz, 0);
	}
	else	{
		size_t rec;
		size_t offs = 0;
		size_t rec_len = file->record_length;

		for (rec = 1; ; rec++)   {
			if (rec > file->record_count) {
				rv = 0;
				break;
			}
			rv = sc_read_record(card, rec, *out + offs + 2, rec_len, SC_RECORD_BY_REC_NR);
			if (rv == SC_ERROR_RECORD_NOT_FOUND)   {
				rv = 0;
				break;
			}
			else if (rv < 0)   {
				break;
			}

			rec_len = rv;

			*(*out + offs) = 'R';
			*(*out + offs + 1) = rv;

			offs += rv + 2;
		}

		sz = offs;
	}

	sc_log(ctx, "read oberthur file result %i", rv);
	if (verify_pin && rv == SC_ERROR_SECURITY_STATUS_NOT_SATISFIED)   {
		struct sc_pkcs15_object *objs[0x10], *pin_obj = NULL;
		const struct sc_acl_entry *acl = sc_file_get_acl_entry(file, SC_AC_OP_READ);
		int ii;

		rv = sc_pkcs15_get_objects(p15card, SC_PKCS15_TYPE_AUTH_PIN, objs, 0x10);
		if (rv != SC_SUCCESS) {
			sc_file_free(file);
			LOG_TEST_RET(ctx, rv, "Cannot read oberthur file: get AUTH objects error");
		}

		for (ii=0; ii<rv; ii++)   {
			struct sc_pkcs15_auth_info *auth_info = (struct sc_pkcs15_auth_info *) objs[ii]->data;
			sc_log(ctx, "compare PIN/ACL refs:%i/%i, method:%i/%i",
					auth_info->attrs.pin.reference, acl->key_ref, auth_info->auth_method, acl->method);
			if (auth_info->attrs.pin.reference == (int)acl->key_ref && auth_info->auth_method == (unsigned)acl->method)   {
				pin_obj = objs[ii];
				break;
			}
		}

		if (!pin_obj || !pin_obj->content.value)    {
			rv = SC_ERROR_SECURITY_STATUS_NOT_SATISFIED;
		}
		else    {
			rv = sc_pkcs15_verify_pin(p15card, pin_obj, pin_obj->content.value, pin_obj->content.len);
			if (!rv)
				rv = sc_oberthur_read_file(p15card, in_path, out, out_len, 0);
		}
	};

	sc_file_free(file);

	if (rv < 0)   {
		free(*out);
		*out = NULL;
		*out_len = 0;
	}

	*out_len = sz;

	LOG_FUNC_RETURN(ctx, rv);
}