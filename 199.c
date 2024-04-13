  OriginDataDeleter(QuotaManager* manager,
                    const GURL& origin,
                    StorageType type,
                    StatusCallback* callback)
      : QuotaTask(manager),
        origin_(origin),
        type_(type),
        error_count_(0),
        remaining_clients_(-1),
        callback_(callback),
        callback_factory_(ALLOW_THIS_IN_INITIALIZER_LIST(this)) {}
