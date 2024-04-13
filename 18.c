void WirelessNetwork::Clear() {
  Network::Clear();
  name_.clear();
  strength_ = 0;
  auto_connect_ = false;
  favorite_ = false;
}
