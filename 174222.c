static path subtract_path(const path &full, const path &prefix)
{
	path::iterator fi = full.begin()
	             , fe = full.end()
	             , pi = prefix.begin()
	             , pe = prefix.end();
	while (fi != fe && pi != pe && *fi == *pi) {
		++fi;
		++pi;
	}
	path rest;
	if (pi == pe)
		while (fi != fe) {
			rest /= *fi;
			++fi;
		}
	return rest;
}