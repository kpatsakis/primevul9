ves_icall_MonoDebugger_GetMethodToken (MonoReflectionMethod *method)
{
	return method->method->token;
}