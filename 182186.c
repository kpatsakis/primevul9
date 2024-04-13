sc_pkcs15_free_prkey(struct sc_pkcs15_prkey *key)
{
	if (!key)
		return;
	switch (key->algorithm) {
	case SC_ALGORITHM_RSA:
		free(key->u.rsa.modulus.data);
		free(key->u.rsa.exponent.data);
		free(key->u.rsa.d.data);
		free(key->u.rsa.p.data);
		free(key->u.rsa.q.data);
		free(key->u.rsa.iqmp.data);
		free(key->u.rsa.dmp1.data);
		free(key->u.rsa.dmq1.data);
		break;
	case SC_ALGORITHM_DSA:
		free(key->u.dsa.pub.data);
		free(key->u.dsa.p.data);
		free(key->u.dsa.q.data);
		free(key->u.dsa.g.data);
		free(key->u.dsa.priv.data);
		break;
	case SC_ALGORITHM_GOSTR3410:
		assert(key->u.gostr3410.d.data);
		free(key->u.gostr3410.d.data);
		break;
	case SC_ALGORITHM_EC:
		if (key->u.ec.params.der.value)
			free(key->u.ec.params.der.value);
		if (key->u.ec.params.named_curve)
			free(key->u.ec.params.named_curve);
		if (key->u.ec.privateD.data)
			free(key->u.ec.privateD.data);
		if (key->u.ec.ecpointQ.value)
			free(key->u.ec.ecpointQ.value);
		break;
	}
}