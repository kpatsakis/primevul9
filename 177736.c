Proxy strToProxy(const char * proxy, bool * ok) {
	Proxy p;
	if (ok) *ok=true;
	//Allow users to use no proxy, even if one is specified in the env
	if (!strcmp(proxy,"none")) {
		p.host = "";
		return p;
	}

	p.type = QNetworkProxy::HttpProxy;
	//Read proxy type bit "http://" or "socks5://"
	if (!strncmp(proxy,"http://",7)) {
		proxy += 7;
	} else if (!strncmp(proxy,"socks5://",9)) {
		p.type = QNetworkProxy::Socks5Proxy;
		proxy += 9;
	}

	//Read username and password
	char * val = (char *) strrchr(proxy,'@');
	p.user = p.password = "";
	if (val != NULL) {
		p.user = QString(proxy).left(val-proxy);
		proxy = val+1;

		int idx = p.user.indexOf(':');
		if (idx != -1) {
			p.password = p.user.mid(idx+1);
			p.user = p.user.left(idx);
		}
	}

 	//Read hostname and port
 	val = (char *) strchr(proxy,':');
 	p.port = 1080; //Default proxy port
 	if (val == NULL) p.host = proxy;
 	else {
		p.port = QString(val+1).toInt(ok);
		if (p.port < 0 || p.port > 65535) {
			p.port = 1080;
			*ok = false;
		}
		p.host = QString(proxy).left(val-proxy);
 	}
	if (ok && p.host.size() == 0) *ok = false;
	return p;
}