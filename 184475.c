JVM_IsNaN(jdouble dbl)
{
	Trc_SC_IsNaN(*(jlong*)&dbl);
	return IS_NAN_DBL(dbl);
}