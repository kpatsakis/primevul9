void QuotaManager::GetHostUsage(const std::string& host, StorageType type,
                                HostUsageCallback* callback) {
  LazyInitialize();
  GetUsageTracker(type)->GetHostUsage(host, callback);
}
