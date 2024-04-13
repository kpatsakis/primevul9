QString proxyToStr(const Proxy & p) {
	QString res="";
	if (p.type == QNetworkProxy::HttpProxy)
		res += "http://";
	else if (p.type == QNetworkProxy::Socks5Proxy)
		res += "socks5://";

	if (!p.user.isEmpty()) {
		res += "@" + p.user;
		if (!p.password.isEmpty()) res += ":" + p.password;
	}
	res += p.host;
	if (!p.host.isEmpty()) res += ":" + QString::number(p.port);
	return res;
}