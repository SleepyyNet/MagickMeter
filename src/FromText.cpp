#include "MagickMeter.h"

BOOL CreateText(ImgStruct &dst, WSVector &setting, Measure * measure)
{
	std::wstring text = setting[0];
	setting.erase(setting.begin());

	dst.contain.fontPointsize(40); //40 * 75% = 30 (default font size)

	double xPos = 0;
	double yPos = 0;
	double skewX = 0;
	double skewY = 0;
	int align = 7;
	double angle = 0;
	int strokeAlign = 0;
	double strokeWidth = 0;
	Magick::Color strokeColor = Magick::Color("black");
	dst.contain.fillColor(Magick::Color("white"));
	Magick::GravityType grav = Magick::NorthWestGravity;

	Magick::Geometry customCanvas;
	size_t clipW = 0;
	size_t clipH = 0;
	size_t clipLine = 0;
	for (int i = 0; i < setting.size(); i++)
	{
		std::wstring tempName, parameter;
		GetNamePara(setting[i], tempName, parameter);
		ParseInternalVariable(measure, parameter, dst);

		LPCWSTR name = tempName.c_str();
		BOOL isSetting = FALSE;
		if (_wcsicmp(name, L"CANVAS") == 0)
		{
			WSVector valList = SeparateList(parameter, L",", 2);
			int tempW = MathParser::ParseI(valList[0]);
			int tempH = MathParser::ParseI(valList[1]);

			if (tempW <= 0 || tempH <= 0)
				RmLog(2, L"Invalid Width or Height value. Default canvas is used.");
			else
			{
				customCanvas = Magick::Geometry((size_t)tempW, (size_t)tempH);
				customCanvas.aspect(true);
			}
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"OFFSET") == 0)
		{
			WSVector valList = SeparateList(parameter, L",", 2);
			xPos = MathParser::ParseF(valList[0]);
			yPos = MathParser::ParseF(valList[1]);
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"ANTIALIAS") == 0)
		{
			dst.contain.textAntiAlias(MathParser::ParseB(parameter));
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"COLOR") == 0)
		{
			dst.contain.fillColor(GetColor(parameter));
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"BACKGROUNDCOLOR") == 0)
		{
			dst.contain.textUnderColor(GetColor(parameter));
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"FACE") == 0)
		{
			std::wstring para = parameter;
			if (para.find(L"@") == 0) //Font file in @Resource folder
			{
				std::wstring fontDir = RmReplaceVariables(measure->rm, L"#@#Fonts\\");
				fontDir += para.substr(1);
				if (std::experimental::filesystem::exists(fontDir))
					dst.contain.font(ws2s(fontDir));
			}
			else if (std::experimental::filesystem::exists(parameter)) //Direct path to font file
			{
				dst.contain.font(ws2s(parameter));
			}
			else //Installed font family name
			{
				dst.contain.fontFamily(ws2s(parameter));
			}
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"SIZE") == 0)
		{
			dst.contain.fontPointsize(MathParser::ParseF(parameter) * 100 / 75); //Rm font size = 75% ours.
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"WEIGHT") == 0)
		{
			dst.contain.fontWeight(MathParser::ParseI(parameter));
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"STYLE") == 0)
		{
			if (_wcsicmp(parameter.c_str(), L"NORMAL") == 0)
				dst.contain.fontStyle(Magick::NormalStyle);

			else if (_wcsicmp(parameter.c_str(), L"ITALIC") == 0)
				dst.contain.fontStyle(Magick::ItalicStyle);

			else if (_wcsicmp(parameter.c_str(), L"OBLIQUE") == 0)
				dst.contain.fontStyle(Magick::ObliqueStyle);
			else
			{
				dst.contain.fontStyle(Magick::NormalStyle);
				RmLog(2, L"Invalid Text Style. Normal style is applied.");
			}
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"CASE") == 0)
		{
			WCHAR* srcAndDest = &text[0];
			int strAndDestLen = (int)text.length();
			if (_wcsicmp(parameter.c_str(), L"UPPER") == 0)
				LCMapString(LOCALE_USER_DEFAULT, LCMAP_UPPERCASE, srcAndDest, strAndDestLen, srcAndDest, strAndDestLen);
			else if (_wcsicmp(parameter.c_str(), L"LOWER") == 0)
				LCMapString(LOCALE_USER_DEFAULT, LCMAP_LOWERCASE, srcAndDest, strAndDestLen, srcAndDest, strAndDestLen);
			else if (_wcsicmp(parameter.c_str(), L"PROPER") == 0)
				LCMapString(LOCALE_USER_DEFAULT, LCMAP_TITLECASE, srcAndDest, strAndDestLen, srcAndDest, strAndDestLen);
			else if (_wcsicmp(parameter.c_str(), L"NONE") == 0)
			{
			}
			else
				RmLogF(measure->rm, 2, L"%s is invalid Text Case.", parameter);

			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"LINESPACING") == 0)
		{
			dst.contain.textInterlineSpacing(MathParser::ParseF(parameter));
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"WORDSPACING") == 0)
		{
			dst.contain.textInterwordSpacing(MathParser::ParseF(parameter));
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"TEXTKERNING") == 0)
		{
			dst.contain.textKerning(MathParser::ParseF(parameter));
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"ALIGN") == 0)
		{
			if (_wcsnicmp(parameter.c_str(), L"LEFTCENTER", 10) == 0)
				align = 1;
			else if (_wcsnicmp(parameter.c_str(), L"LEFTBOTTOM", 10) == 0)
				align = 2;
			else if (_wcsnicmp(parameter.c_str(), L"RIGHTCENTER", 11) == 0)
				align = 3;
			else if (_wcsnicmp(parameter.c_str(), L"RIGHTBOTTOM", 11) == 0)
				align = 4;
			else if (_wcsnicmp(parameter.c_str(), L"CENTERCENTER", 11) == 0)
				align = 5;
			else if (_wcsnicmp(parameter.c_str(), L"CENTERBOTTOM", 12) == 0)
				align = 6;
			else if (_wcsnicmp(parameter.c_str(), L"LEFTTOP", 7) == 0 || _wcsnicmp(parameter.c_str(), L"LEFT", 4) == 0)
				align = 7;
			else if (_wcsnicmp(parameter.c_str(), L"RIGHTTOP", 8) == 0 || _wcsnicmp(parameter.c_str(), L"RIGHT", 5) == 0)
				align = 8;
			else if (_wcsnicmp(parameter.c_str(), L"CENTERTOP", 9) == 0 || _wcsnicmp(parameter.c_str(), L"CENTER", 6) == 0)
				align = 9;
			else
				RmLog(2, L"Invalid Align value. Anchor Left is applied");

			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"SKEW") == 0)
		{
			WSVector valList = SeparateList(parameter, L",", 2);
			skewX = MathParser::ParseF(valList[0]);
			dst.contain.transformSkewX(skewX);

			skewY = MathParser::ParseF(valList[1]);
			dst.contain.transformSkewY(skewY);
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"DIRECTION") == 0)
		{
			Magick::DirectionType dir = MagickCore::LeftToRightDirection;
			if (_wcsicmp(parameter.c_str(), L"LEFTTORIGHT") == 0)
				dir = MagickCore::LeftToRightDirection;
			else if (_wcsicmp(parameter.c_str(), L"RIGHTTOLEFT") == 0)
				dir = MagickCore::RightToLeftDirection;
			else
				RmLog(2, L"Invalid Direction value. Left to Right direction is applied");

			dst.contain.textDirection(dir);
			isSetting = TRUE;
		}
		if (_wcsicmp(name, L"STROKECOLOR") == 0)
		{
			dst.contain.strokeColor(GetColor(parameter));
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"STROKEWIDTH") == 0)
		{
			strokeWidth = MathParser::ParseF(parameter);
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"DENSITY") == 0)
		{
			dst.contain.density(MathParser::ParseF(parameter));
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"CLIPSTRINGW") == 0)
		{
			clipW = (size_t)abs(MathParser::ParseI(parameter));
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"CLIPSTRINGH") == 0)
		{
			clipH = (size_t)abs(MathParser::ParseI(parameter));
			isSetting = TRUE;
		}
		else if (_wcsicmp(name, L"CLIPSTRINGLINE") == 0)
		{
			clipLine = (size_t)abs(MathParser::ParseI(parameter));
			isSetting = TRUE;
		}
		/*else if (_wcsicmp(name, L"STROKEALIGN") == 0)
		{
		if (_wcsicmp(parameter.c_str(), L"CENTER") == 0)
		strokeAlign = 0;

		else if (_wcsicmp(parameter.c_str(), L"OUTSIDE") == 0)
		strokeAlign = 1;

		else if (_wcsicmp(parameter.c_str(), L"INSIDE") == 0)
		strokeAlign = 2;
		else
		{
		strokeAlign = 0;
		RmLog(2, L"Invalid StrokeAlign value. Center align is applied.");
		}

		isSetting = TRUE;
		}*/
		if (isSetting)
			setting[i] = L"";
	}

	try
	{
		Magick::TypeMetric metric;

		dst.contain.fontTypeMetricsMultiline(ws2s(text), &metric);

		if (clipW != 0)
		{
			if (metric.textWidth() > clipW)
			{
				WSVector words = SeparateList(text, L" ", NULL);

				WSVector lines;
				UINT offset = 1;
				UINT startLine = 0;
				while (startLine + offset <= words.size())
				{
					std::wstring curLine = L"";
					for (UINT j = startLine; j < startLine + offset; j++)
						curLine += words[j] + L" ";

					dst.contain.fontTypeMetricsMultiline(ws2s(curLine), &metric);
					if ((size_t)ceil(metric.textWidth()) < clipW)
						offset++;
					else
					{
						std::wstring addLine = L"";
						if (offset == 1)
						{
							addLine = words[startLine];
							if (metric.textWidth() > clipW)
							{
								std::wstring w = L"";
								std::wstring oldw = L"";
								for (int c = 0; c < addLine.length(); c++)
								{
									oldw = w;
									w += addLine[c];
									dst.contain.fontTypeMetricsMultiline(ws2s(w + L"-"), &metric);
									if (metric.textWidth() > clipW)
									{
										w = L"";
										for (int rC = c; rC < addLine.length(); rC++)
											w += words[startLine][rC];

										words[startLine] = w;
										addLine = oldw + L"-";
										break;
									}
								}
							}
							else
								offset++;
						}
						else
							for (UINT j = startLine; j < startLine + offset - 1; j++)
								addLine += words[j] + L" ";

						lines.push_back(addLine);
						startLine += offset - 1;
						offset = 1;
					}
				}
				if (startLine < words.size())
				{
					std::wstring addLine = L"";
					for (UINT i = startLine; i < words.size(); i++)
						addLine += words[i] + L" ";
					lines.push_back(addLine);
				}
				if (lines.size() >= 1)
				text = lines[0];
				for (size_t i = 1; i < lines.size(); i++)
					text += L"\n" + lines[i];
			}

		}

		if (clipH != 0)
		{
			WSVector lineList = SeparateList(text, L"\n", NULL);
			size_t listSize = lineList.size();
			if (listSize > 1)
			{
				std::wstring curText = lineList[0];
				size_t lastLine = 0;
				for (size_t i = 1; i < listSize; i++)
				{
					dst.contain.fontTypeMetricsMultiline(ws2s(curText + L"\n" + lineList[i]), &metric);
					if (metric.textHeight() <= clipH)
					{
						curText += L"\n" + lineList[i];
						lastLine = i;
					}
					else
						break;
				}

				if (lastLine != (listSize - 1))
				{
					curText += L"...";
					if (clipW != 0)
						for (size_t i = curText.length() - 1; i >= 2; i--)
						{
							curText[i] = L'.';
							curText[i-1] = L'.';
							curText[i-2] = L'.';
							if (i != curText.length() - 1) curText.pop_back();
							dst.contain.fontTypeMetricsMultiline(ws2s(curText), &metric);
							if (metric.textWidth() <= clipW)
								break;
						}
					text = curText;
				}
			}
		}

		if (clipLine != 0)
		{
			WSVector lineList = SeparateList(text, L"\n", NULL);
			size_t listSize = lineList.size();
			if (listSize > 1 && clipLine < listSize)
			{
				std::wstring curText = lineList[0];
				for (size_t i = 1; i < clipLine; i++)
					curText += L"\n" + lineList[i];

				curText += L"...";
				if (clipW != 0)
					for (size_t i = curText.length() - 1; i >= 2; i--)
					{
						if (i != curText.length() - 1)
						{
							curText.pop_back();
							curText[i] = L'.';
							curText[i - 1] = L'.';
							curText[i - 2] = L'.';
						}

						dst.contain.fontTypeMetricsMultiline(ws2s(curText), &metric);
						if (metric.textWidth() <= clipW)
							break;
					}
				text = curText;
			}
		}

		dst.contain.fontTypeMetricsMultiline(ws2s(text), &metric);

		dst.contain.strokeWidth(strokeWidth);

		//Some fonts have glyphs that overflows normal zone
		//and fontTypeMetric doesn't detect that.
		//So we have to offset a textHeight() amount.
		Magick::Geometry tempSize(
			(size_t)ceil(metric.textWidth() + strokeWidth / 2 + metric.textHeight() * 2),
			(size_t)ceil(metric.textHeight() + strokeWidth / 2 + metric.textHeight() * 2),
			(ssize_t)ceil(metric.textHeight()), (ssize_t)ceil(metric.textHeight())
		);
		tempSize.aspect(true);

		Magick::Image tempImg = dst.contain;

		switch (align)
		{
		case 3:
		case 4:
		case 8:
			grav = Magick::NorthEastGravity;
			break;
		case 5:
		case 6:
		case 9:
			grav = Magick::NorthGravity;
			break;
		}
		tempImg.scale(tempSize);
		tempImg.annotate(ws2s(text), tempSize, grav);
		tempImg.crop(tempImg.boundingBox());

		dst.W = (size_t)ceil(tempImg.columns());
		dst.H = (size_t)ceil(tempImg.rows());

		switch (align)
		{
		case 1:
			yPos -= tempImg.rows() / 2;
			break;
		case 2:
			yPos -= tempImg.rows();
			break;
		case 3:
			xPos -= tempImg.columns();
			yPos -= tempImg.rows() / 2;
			break;
		case 4:
			xPos -= tempImg.columns();
			yPos -= tempImg.rows();
			break;
		case 5:
			xPos -= tempImg.columns() / 2;
			yPos -= tempImg.rows() / 2;
			break;
		case 6:
			xPos -= tempImg.columns() / 2;
			yPos -= tempImg.rows();
			break;
		case 8:
			xPos -= tempImg.columns();
			break;
		case 9:
			xPos -= tempImg.columns() / 2;
			break;
		}

		dst.X = (ssize_t)round(xPos);
		dst.Y = (ssize_t)round(yPos);

		if (!customCanvas.isValid())
		{
			customCanvas = Magick::Geometry(
				dst.X + dst.W,
				dst.Y + dst.H
			);
			customCanvas.aspect(true);
		}

		dst.contain.scale(customCanvas);

		dst.contain.composite(tempImg, dst.X, dst.Y, MagickCore::OverCompositeOp);
	}
	catch (Magick::Exception &error_)
	{
		error2pws(error_);
		return FALSE;
	}

	return TRUE;
}

/*STROKE ALIGN
Magick::TypeMetric metrics;
if (strokeWidth == 0)
{
dst.contain.fontTypeMetricsMultiline(ws2s(text), &metrics);
Magick::Geometry newSize(
(size_t)(metrics.textWidth() + 200),
(size_t)(metrics.textHeight() + 200)
);
newSize.aspect(true);
dst.contain.resize(newSize);

dst.contain.annotate(ws2s(text), Magick::Geometry(0, 0, 200, 200));
}
else if (strokeWidth != 0 && strokeAlign != 0)
{
dst.contain.strokeWidth(strokeWidth * 2);
dst.contain.strokeColor(strokeColor);

dst.contain.fontTypeMetricsMultiline(ws2s(text), &metrics);

Magick::Geometry newSize(
(size_t)(metrics.textWidth()),
(size_t)(metrics.textHeight())
);
newSize.aspect(true);
dst.contain.resize(newSize);

dst.contain.annotate(ws2s(text), newSize, Magick::NorthGravity);
Magick::Image temp = dst.contain;
temp.strokeColor(INVISIBLE);
temp.annotate(ws2s(text), newSize, Magick::NorthGravity);

if (strokeAlign == 1)
{
dst.contain.composite(temp, 0, 0, MagickCore::OverCompositeOp);
}
else if (strokeAlign == 2)
{
dst.contain.composite(temp, 0, 0, MagickCore::CopyAlphaCompositeOp);
}
}
else
{
dst.contain.strokeWidth(strokeWidth);
dst.contain.strokeColor(strokeColor);
dst.contain.fontTypeMetricsMultiline(ws2s(text), &metrics);

Magick::Geometry newSize(
(size_t)(metrics.textWidth()),
(size_t)(metrics.textHeight())
);
newSize.aspect(true);
dst.contain.resize(newSize);

dst.contain.annotate(ws2s(text), newSize, Magick::NorthGravity);

}*/