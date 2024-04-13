void MsgSetInputName(msg_t *pThis, prop_t *inputName)
{
	assert(pThis != NULL);

	prop.AddRef(inputName);
	if(pThis->pInputName != NULL)
		prop.Destruct(&pThis->pInputName);
	pThis->pInputName = inputName;
}