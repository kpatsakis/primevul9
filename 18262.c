PortForwardSourceResponse PortForwardHandler::createSource(
    const PortForwardSourceRequest& pfsr, string* sourceName, uid_t userid,
    gid_t groupid) {
  try {
    if (pfsr.has_source() && sourceName) {
      throw runtime_error(
          "Do not set a source when forwarding named pipes with environment "
          "variables");
    }
    SocketEndpoint source;
    if (pfsr.has_source()) {
      source = pfsr.source();
      if (source.has_name()) {
        throw runtime_error(
            "Named socket tunneling is only allowed with temporary filenames.");
      }
    } else {
      // Make a random file to forward the pipe
      string sourcePattern =
          GetTempDirectory() + string("et_forward_sock_XXXXXX");
      string sourceDirectory = string(mkdtemp(&sourcePattern[0]));
      FATAL_FAIL(::chmod(sourceDirectory.c_str(), S_IRUSR | S_IWUSR | S_IXUSR));
      FATAL_FAIL(::chown(sourceDirectory.c_str(), userid, groupid));
      string sourcePath = string(sourceDirectory) + "/sock";

      source.set_name(sourcePath);
      if (sourceName == nullptr) {
        STFATAL
            << "Tried to create a pipe but without a place to put the name!";
      }
      *sourceName = sourcePath;
      LOG(INFO) << "Creating pipe at " << sourcePath;
    }
    if (pfsr.source().has_port()) {
      if (sourceName != nullptr) {
        STFATAL << "Tried to create a port forward but with a place to put "
                   "the name!";
      }
      auto handler = shared_ptr<ForwardSourceHandler>(new ForwardSourceHandler(
          networkSocketHandler, source, pfsr.destination()));
      sourceHandlers.push_back(handler);
      return PortForwardSourceResponse();
    } else {
      if (userid < 0 || groupid < 0) {
        STFATAL
            << "Tried to create a unix socket forward with no userid/groupid";
      }
      auto handler = shared_ptr<ForwardSourceHandler>(new ForwardSourceHandler(
          pipeSocketHandler, source, pfsr.destination()));
      FATAL_FAIL(::chmod(source.name().c_str(), S_IRUSR | S_IWUSR | S_IXUSR));
      FATAL_FAIL(::chown(source.name().c_str(), userid, groupid));
      sourceHandlers.push_back(handler);
      return PortForwardSourceResponse();
    }
  } catch (const std::runtime_error& ex) {
    PortForwardSourceResponse pfsr;
    pfsr.set_error(ex.what());
    return pfsr;
  }
}