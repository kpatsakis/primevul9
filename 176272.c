find_type_in_ad(krb5_context context,
		int type,
		krb5_data *data,
		krb5_boolean *found,
		krb5_boolean failp,
		krb5_keyblock *sessionkey,
		const AuthorizationData *ad,
		int level)
{
    krb5_error_code ret = 0;
    size_t i;

    if (level > 9) {
	ret = ENOENT; /* XXX */
	krb5_set_error_message(context, ret,
			       N_("Authorization data nested deeper "
				  "then %d levels, stop searching", ""),
			       level);
	goto out;
    }

    /*
     * Only copy out the element the first time we get to it, we need
     * to run over the whole authorization data fields to check if
     * there are any container clases we need to care about.
     */
    for (i = 0; i < ad->len; i++) {
	if (!*found && ad->val[i].ad_type == type) {
	    ret = der_copy_octet_string(&ad->val[i].ad_data, data);
	    if (ret) {
		krb5_set_error_message(context, ret,
				       N_("malloc: out of memory", ""));
		goto out;
	    }
	    *found = TRUE;
	    continue;
	}
	switch (ad->val[i].ad_type) {
	case KRB5_AUTHDATA_IF_RELEVANT: {
	    AuthorizationData child;
	    ret = decode_AuthorizationData(ad->val[i].ad_data.data,
					   ad->val[i].ad_data.length,
					   &child,
					   NULL);
	    if (ret) {
		krb5_set_error_message(context, ret,
				       N_("Failed to decode "
					  "IF_RELEVANT with %d", ""),
				       (int)ret);
		goto out;
	    }
	    ret = find_type_in_ad(context, type, data, found, FALSE,
				  sessionkey, &child, level + 1);
	    free_AuthorizationData(&child);
	    if (ret)
		goto out;
	    break;
	}
#if 0 /* XXX test */
	case KRB5_AUTHDATA_KDC_ISSUED: {
	    AD_KDCIssued child;

	    ret = decode_AD_KDCIssued(ad->val[i].ad_data.data,
				      ad->val[i].ad_data.length,
				      &child,
				      NULL);
	    if (ret) {
		krb5_set_error_message(context, ret,
				       N_("Failed to decode "
					  "AD_KDCIssued with %d", ""),
				       ret);
		goto out;
	    }
	    if (failp) {
		krb5_boolean valid;
		krb5_data buf;
		size_t len;

		ASN1_MALLOC_ENCODE(AuthorizationData, buf.data, buf.length,
				   &child.elements, &len, ret);
		if (ret) {
		    free_AD_KDCIssued(&child);
		    krb5_clear_error_message(context);
		    goto out;
		}
		if(buf.length != len)
		    krb5_abortx(context, "internal error in ASN.1 encoder");

		ret = krb5_c_verify_checksum(context, sessionkey, 19, &buf,
					     &child.ad_checksum, &valid);
		krb5_data_free(&buf);
		if (ret) {
		    free_AD_KDCIssued(&child);
		    goto out;
		}
		if (!valid) {
		    krb5_clear_error_message(context);
		    ret = ENOENT;
		    free_AD_KDCIssued(&child);
		    goto out;
		}
	    }
	    ret = find_type_in_ad(context, type, data, found, failp, sessionkey,
				  &child.elements, level + 1);
	    free_AD_KDCIssued(&child);
	    if (ret)
		goto out;
	    break;
	}
#endif
	case KRB5_AUTHDATA_AND_OR:
	    if (!failp)
		break;
	    ret = ENOENT; /* XXX */
	    krb5_set_error_message(context, ret,
				   N_("Authorization data contains "
				      "AND-OR element that is unknown to the "
				      "application", ""));
	    goto out;
	default:
	    if (!failp)
		break;
	    ret = ENOENT; /* XXX */
	    krb5_set_error_message(context, ret,
				   N_("Authorization data contains "
				      "unknown type (%d) ", ""),
				   ad->val[i].ad_type);
	    goto out;
	}
    }
out:
    if (ret) {
	if (*found) {
	    krb5_data_free(data);
	    *found = 0;
	}
    }
    return ret;
}