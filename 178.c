void QuotaManager::GetTemporaryGlobalQuota(QuotaCallback* callback) {
  LazyInitialize();
  if (temporary_global_quota_ >= 0) {
    callback->Run(kQuotaStatusOk,
                  kStorageTypeTemporary, temporary_global_quota_);
    delete callback;
    return;
  }
  temporary_global_quota_callbacks_.Add(callback);
}
