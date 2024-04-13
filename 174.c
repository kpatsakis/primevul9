  GetModifiedSinceTask(
      QuotaManager* manager,
      StorageType type,
      base::Time modified_since,
      GetOriginsCallback* callback)
      : DatabaseTaskBase(manager),
        type_(type),
        modified_since_(modified_since),
        callback_(callback) {}
