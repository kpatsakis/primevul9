static int nsh_key_put_from_nlattr(const struct nlattr *attr,
				   struct sw_flow_match *match, bool is_mask,
				   bool is_push_nsh, bool log)
{
	struct nlattr *a;
	int rem;
	bool has_base = false;
	bool has_md1 = false;
	bool has_md2 = false;
	u8 mdtype = 0;
	int mdlen = 0;

	if (WARN_ON(is_push_nsh && is_mask))
		return -EINVAL;

	nla_for_each_nested(a, attr, rem) {
		int type = nla_type(a);
		int i;

		if (type > OVS_NSH_KEY_ATTR_MAX) {
			OVS_NLERR(log, "nsh attr %d is out of range max %d",
				  type, OVS_NSH_KEY_ATTR_MAX);
			return -EINVAL;
		}

		if (!check_attr_len(nla_len(a),
				    ovs_nsh_key_attr_lens[type].len)) {
			OVS_NLERR(
			    log,
			    "nsh attr %d has unexpected len %d expected %d",
			    type,
			    nla_len(a),
			    ovs_nsh_key_attr_lens[type].len
			);
			return -EINVAL;
		}

		switch (type) {
		case OVS_NSH_KEY_ATTR_BASE: {
			const struct ovs_nsh_key_base *base = nla_data(a);

			has_base = true;
			mdtype = base->mdtype;
			SW_FLOW_KEY_PUT(match, nsh.base.flags,
					base->flags, is_mask);
			SW_FLOW_KEY_PUT(match, nsh.base.ttl,
					base->ttl, is_mask);
			SW_FLOW_KEY_PUT(match, nsh.base.mdtype,
					base->mdtype, is_mask);
			SW_FLOW_KEY_PUT(match, nsh.base.np,
					base->np, is_mask);
			SW_FLOW_KEY_PUT(match, nsh.base.path_hdr,
					base->path_hdr, is_mask);
			break;
		}
		case OVS_NSH_KEY_ATTR_MD1: {
			const struct ovs_nsh_key_md1 *md1 = nla_data(a);

			has_md1 = true;
			for (i = 0; i < NSH_MD1_CONTEXT_SIZE; i++)
				SW_FLOW_KEY_PUT(match, nsh.context[i],
						md1->context[i], is_mask);
			break;
		}
		case OVS_NSH_KEY_ATTR_MD2:
			if (!is_push_nsh) /* Not supported MD type 2 yet */
				return -ENOTSUPP;

			has_md2 = true;
			mdlen = nla_len(a);
			if (mdlen > NSH_CTX_HDRS_MAX_LEN || mdlen <= 0) {
				OVS_NLERR(
				    log,
				    "Invalid MD length %d for MD type %d",
				    mdlen,
				    mdtype
				);
				return -EINVAL;
			}
			break;
		default:
			OVS_NLERR(log, "Unknown nsh attribute %d",
				  type);
			return -EINVAL;
		}
	}

	if (rem > 0) {
		OVS_NLERR(log, "nsh attribute has %d unknown bytes.", rem);
		return -EINVAL;
	}

	if (has_md1 && has_md2) {
		OVS_NLERR(
		    1,
		    "invalid nsh attribute: md1 and md2 are exclusive."
		);
		return -EINVAL;
	}

	if (!is_mask) {
		if ((has_md1 && mdtype != NSH_M_TYPE1) ||
		    (has_md2 && mdtype != NSH_M_TYPE2)) {
			OVS_NLERR(1, "nsh attribute has unmatched MD type %d.",
				  mdtype);
			return -EINVAL;
		}

		if (is_push_nsh &&
		    (!has_base || (!has_md1 && !has_md2))) {
			OVS_NLERR(
			    1,
			    "push_nsh: missing base or metadata attributes"
			);
			return -EINVAL;
		}
	}

	return 0;
}