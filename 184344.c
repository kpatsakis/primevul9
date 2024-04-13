void MonClient::register_config_callback(md_config_t::config_callback fn) {
  ceph_assert(!config_cb);
  config_cb = fn;
}