AvahiServerConfig* avahi_server_config_init(AvahiServerConfig *c) {
    assert(c);

    memset(c, 0, sizeof(AvahiServerConfig));
    c->use_ipv6 = 1;
    c->use_ipv4 = 1;
    c->allow_interfaces = NULL;
    c->deny_interfaces = NULL;
    c->host_name = NULL;
    c->domain_name = NULL;
    c->check_response_ttl = 0;
    c->publish_hinfo = 1;
    c->publish_addresses = 1;
    c->publish_workstation = 1;
    c->publish_domain = 1;
    c->use_iff_running = 0;
    c->enable_reflector = 0;
    c->reflect_ipv = 0;
    c->add_service_cookie = 0;
    c->enable_wide_area = 0;
    c->n_wide_area_servers = 0;
    c->disallow_other_stacks = 0;
    c->browse_domains = NULL;
    c->disable_publishing = 0;
    c->allow_point_to_point = 0;
    c->publish_aaaa_on_ipv4 = 1;
    c->publish_a_on_ipv6 = 0;

    return c;
}