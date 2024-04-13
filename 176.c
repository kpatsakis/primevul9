  GetPersistentHostQuotaTask(
      QuotaManager* manager,
      const std::string& host,
      HostQuotaCallback* callback)
      : DatabaseTaskBase(manager),
        host_(host),
        quota_(-1),
        callback_(callback) {
  }
