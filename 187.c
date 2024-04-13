  HostQuotaCallback* NewWaitableHostQuotaCallback() {
    ++waiting_callbacks_;
    return callback_factory_.NewCallback(
            &UsageAndQuotaDispatcherTask::DidGetHostQuota);
  }
