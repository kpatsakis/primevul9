void Network::Clear() {
  state_ = STATE_UNKNOWN;
  error_ = ERROR_UNKNOWN;
  service_path_.clear();
  device_path_.clear();
  ip_address_.clear();
  is_active_ = false;
}
