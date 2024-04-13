respip_action_cfg(struct respip_set* set, const char* ipstr,
	const char* actnstr)
{
	struct resp_addr* node;
	enum respip_action action;

	if(!(node=respip_find_or_create(set, ipstr, 1)))
		return 0;
	if(node->action != respip_none) {
		verbose(VERB_QUERY, "duplicate response-ip action for '%s', overridden.",
			ipstr);
	}
        if(strcmp(actnstr, "deny") == 0)
                action = respip_deny;
        else if(strcmp(actnstr, "redirect") == 0)
                action = respip_redirect;
        else if(strcmp(actnstr, "inform") == 0)
                action = respip_inform;
        else if(strcmp(actnstr, "inform_deny") == 0)
                action = respip_inform_deny;
        else if(strcmp(actnstr, "inform_redirect") == 0)
                action = respip_inform_redirect;
        else if(strcmp(actnstr, "always_transparent") == 0)
                action = respip_always_transparent;
        else if(strcmp(actnstr, "always_refuse") == 0)
                action = respip_always_refuse;
        else if(strcmp(actnstr, "always_nxdomain") == 0)
                action = respip_always_nxdomain;
        else {
                log_err("unknown response-ip action %s", actnstr);
                return 0;
        }
	node->action = action;
	return 1;
}