void CGraph::Render(IGraphics *pGraphics, int Font, float x, float y, float w, float h, const char *pDescription)
{
	//m_pGraphics->BlendNormal();


	pGraphics->TextureSet(-1);

	pGraphics->QuadsBegin();
	pGraphics->SetColor(0, 0, 0, 0.75f);
	IGraphics::CQuadItem QuadItem(x, y, w, h);
	pGraphics->QuadsDrawTL(&QuadItem, 1);
	pGraphics->QuadsEnd();

	pGraphics->LinesBegin();
	pGraphics->SetColor(0.95f, 0.95f, 0.95f, 1.00f);
	IGraphics::CLineItem LineItem(x, y+h/2, x+w, y+h/2);
	pGraphics->LinesDraw(&LineItem, 1);
	pGraphics->SetColor(0.5f, 0.5f, 0.5f, 0.75f);
	IGraphics::CLineItem Array[2] = {
		IGraphics::CLineItem(x, y+(h*3)/4, x+w, y+(h*3)/4),
		IGraphics::CLineItem(x, y+h/4, x+w, y+h/4)};
	pGraphics->LinesDraw(Array, 2);
	for(int i = 1; i < MAX_VALUES; i++)
	{
		float a0 = (i-1)/(float)MAX_VALUES;
		float a1 = i/(float)MAX_VALUES;
		int i0 = (m_Index+i-1)&(MAX_VALUES-1);
		int i1 = (m_Index+i)&(MAX_VALUES-1);

		float v0 = (m_aValues[i0]-m_Min) / (m_Max-m_Min);
		float v1 = (m_aValues[i1]-m_Min) / (m_Max-m_Min);

		IGraphics::CColorVertex Array[2] = {
			IGraphics::CColorVertex(0, m_aColors[i0][0], m_aColors[i0][1], m_aColors[i0][2], 0.75f),
			IGraphics::CColorVertex(1, m_aColors[i1][0], m_aColors[i1][1], m_aColors[i1][2], 0.75f)};
		pGraphics->SetColorVertex(Array, 2);
		IGraphics::CLineItem LineItem(x+a0*w, y+h-v0*h, x+a1*w, y+h-v1*h);
		pGraphics->LinesDraw(&LineItem, 1);

	}
	pGraphics->LinesEnd();

	pGraphics->TextureSet(Font);
	pGraphics->QuadsBegin();
	pGraphics->QuadsText(x+2, y+h-16, 16, pDescription);

	char aBuf[32];
	str_format(aBuf, sizeof(aBuf), "%.2f", m_Max);
	pGraphics->QuadsText(x+w-8*str_length(aBuf)-8, y+2, 16, aBuf);

	str_format(aBuf, sizeof(aBuf), "%.2f", m_Min);
	pGraphics->QuadsText(x+w-8*str_length(aBuf)-8, y+h-16, 16, aBuf);
	pGraphics->QuadsEnd();
}