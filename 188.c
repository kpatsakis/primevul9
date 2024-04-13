  HostUsageCallback* NewWaitableHostUsageCallback() {
    ++waiting_callbacks_;
    return callback_factory_.NewCallback(
            &UsageAndQuotaDispatcherTask::DidGetHostUsage);
  }
