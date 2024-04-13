static void komeda_wb_connector_destroy(struct drm_connector *connector)
{
	drm_connector_cleanup(connector);
	kfree(to_kconn(to_wb_conn(connector)));
}