duplicate_DevicePropDesc(const PTPDevicePropDesc *src, PTPDevicePropDesc *dst) {
	int i;

	dst->DevicePropertyCode	= src->DevicePropertyCode;
	dst->DataType		= src->DataType;
	dst->GetSet		= src->GetSet;
	
	duplicate_PropertyValue (&src->FactoryDefaultValue, &dst->FactoryDefaultValue, src->DataType);
	duplicate_PropertyValue (&src->CurrentValue, &dst->CurrentValue, src->DataType);

	dst->FormFlag		= src->FormFlag;
	switch (src->FormFlag) {
	case PTP_DPFF_Range:
		duplicate_PropertyValue (&src->FORM.Range.MinimumValue, &dst->FORM.Range.MinimumValue, src->DataType);
		duplicate_PropertyValue (&src->FORM.Range.MaximumValue, &dst->FORM.Range.MaximumValue, src->DataType);
		duplicate_PropertyValue (&src->FORM.Range.StepSize,     &dst->FORM.Range.StepSize,     src->DataType);
		break;
	case PTP_DPFF_Enumeration:
		dst->FORM.Enum.NumberOfValues = src->FORM.Enum.NumberOfValues;
		dst->FORM.Enum.SupportedValue = malloc (sizeof(dst->FORM.Enum.SupportedValue[0])*src->FORM.Enum.NumberOfValues);
		for (i = 0; i<src->FORM.Enum.NumberOfValues ; i++)
			duplicate_PropertyValue (&src->FORM.Enum.SupportedValue[i], &dst->FORM.Enum.SupportedValue[i], src->DataType);
		break;
	case PTP_DPFF_None:
		break;
	}
}