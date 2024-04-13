ves_icall_System_Char_GetDataTablePointers (guint8 const **category_data,
					    guint8 const **numeric_data,
					    gdouble const **numeric_data_values,
					    guint16 const **to_lower_data_low,
					    guint16 const **to_lower_data_high,
					    guint16 const **to_upper_data_low,
					    guint16 const **to_upper_data_high)
{
	*category_data = CategoryData;
	*numeric_data = NumericData;
	*numeric_data_values = NumericDataValues;
	*to_lower_data_low = ToLowerDataLow;
	*to_lower_data_high = ToLowerDataHigh;
	*to_upper_data_low = ToUpperDataLow;
	*to_upper_data_high = ToUpperDataHigh;
}