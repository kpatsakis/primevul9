update_log_cb(void *arg, dns_zone_t *zone, int level, const char *message) {
	UNUSED(arg);
	dns_zone_log(zone, level, "%s", message);
}