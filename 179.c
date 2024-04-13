 void QuotaManager::GetUsageAndQuota(
     const GURL& origin, StorageType type,
     GetUsageAndQuotaCallback* callback_ptr) {
  scoped_ptr<GetUsageAndQuotaCallback> callback(callback_ptr);
  LazyInitialize();

  if (type == kStorageTypeUnknown) {
    callback->Run(kQuotaErrorNotSupported, 0, 0);
    return;
  }

  std::string host = net::GetHostOrSpecFromURL(origin);
  UsageAndQuotaDispatcherTaskMap::iterator found =
      usage_and_quota_dispatchers_.find(std::make_pair(host, type));
  if (found == usage_and_quota_dispatchers_.end()) {
    UsageAndQuotaDispatcherTask* dispatcher =
        UsageAndQuotaDispatcherTask::Create(this, host, type);
    found = usage_and_quota_dispatchers_.insert(
        std::make_pair(std::make_pair(host, type), dispatcher)).first;
  }
  if (found->second->AddCallback(
          callback.release(), IsStorageUnlimited(origin))) {
    found->second->Start();
  }
}
