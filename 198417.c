void sc_pkcs15emu_sc_hsm_free_cvc(sc_cvc_t *cvc)
{
	if (cvc->signature) {
		free(cvc->signature);
		cvc->signature = NULL;
	}
	if (cvc->primeOrModulus) {
		free(cvc->primeOrModulus);
		cvc->primeOrModulus = NULL;
	}
	if (cvc->coefficientAorExponent) {
		free(cvc->coefficientAorExponent);
		cvc->coefficientAorExponent = NULL;
	}
	if (cvc->coefficientB) {
		free(cvc->coefficientB);
		cvc->coefficientB = NULL;
	}
	if (cvc->basePointG) {
		free(cvc->basePointG);
		cvc->basePointG = NULL;
	}
	if (cvc->order) {
		free(cvc->order);
		cvc->order = NULL;
	}
	if (cvc->publicPoint) {
		free(cvc->publicPoint);
		cvc->publicPoint = NULL;
	}
	if (cvc->cofactor) {
		free(cvc->cofactor);
		cvc->cofactor = NULL;
	}
}