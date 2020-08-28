#include "CDocumentStyle.h"

#include <iostream>
#include <algorithm>
#include <wchar.h>

struct ParentStyle
{
    CXmlElement oParentsXmlElement;
    std::wstring oNameParent;
};

bool Comp(const std::pair<NSCSS::CCompiledStyle, bool> &oFirstElement, const std::pair<NSCSS::CCompiledStyle, bool> &oSecondElement)
{
    return oFirstElement.first.GetSize() < oSecondElement.first.GetSize();
}

namespace NSCSS
{
CDocumentStyle::CDocumentStyle()
    {
        m_arStandardStyles = {L"a",         L"li",
                              L"h1",        L"h2",
                              L"h3",        L"h4",
                              L"h5",        L"h6",
                              L"title1-c",  L"title2-c",
                              L"title3-c",  L"title4-c",
                              L"title5-c",  L"title6-c",
                              L"p-c",       L"p",
                              L"div-c",     L"div"};
    }

    CDocumentStyle::~CDocumentStyle()
    {
    }

    std::wstring CDocumentStyle::GetStyle()
    {
        if (m_sId.empty())
            return L"";
        std::wstring sStyle = m_sStyle;
        Clear();
        return sStyle;
    }

    std::wstring CDocumentStyle::GetId() const
    {
        return m_sId;
    }

    void CDocumentStyle::Clear()
    {
        m_sId.clear();
        m_sStyle.clear();
    }

    void CDocumentStyle::SetStyle(const std::wstring &sStyle)
    {
        if (sStyle.empty())
            return;
        m_sStyle = sStyle;
    }

    void CDocumentStyle::SetId(const std::wstring &sId)
    {
        if (sId.empty())
            return;
        m_sId = sId;
    }

    CXmlElement CDocumentStyle::CombineStandardStyles(const std::vector<std::wstring> arStandartedStyles)
    {
        if (arStandartedStyles.size() == 0)
        {
            CXmlElement oEmpty;
            oEmpty.Clear();
            return oEmpty;
        }
        std::vector<std::wstring> arStyles;

        for (std::wstring sStyleName : arStandartedStyles)
        {
            if (std::find(m_arStandardStyles.begin(), m_arStandardStyles.end(), sStyleName) != m_arStandardStyles.cend() &&
                std::find(arStyles.begin(), arStyles.end(), sStyleName) == arStyles.cend())
            {
                arStyles.push_back(sStyleName);
            }
        }

        CXmlElement oXmlElement;
        oXmlElement.Clear();

        if (arStyles.empty())
            return oXmlElement;

        std::wstring sId;

        for (std::wstring sStyleName : arStyles)
        {
            sId += sStyleName;
            if (sStyleName != arStyles[arStyles.size() - 1])
                sId += L'+';
        }

        for (std::wstring sStyleName : arStyles)
        {
            CXmlElement oTempXmlElement = CreateStandardStyle(sStyleName);
            if (!oTempXmlElement.Empty())
            {
                oXmlElement += oTempXmlElement;
//                m_arStandardStylesUsed.push_back(sStyleName);
            }
        }

        if (!oXmlElement.Empty())
        {
            oXmlElement.SetName(sId);
            oXmlElement.SetStyleId(sId);
        }
        else oXmlElement.Clear();

        return oXmlElement;
    }

    CXmlElement CDocumentStyle::CreateStandardStyle(const std::wstring sNameStyle)
    {
        CXmlElement oXmlElement;

        if (std::find(m_arStandardStyles.begin(), m_arStandardStyles.end(), sNameStyle) == m_arStandardStyles.cend())
        {
            oXmlElement.Clear();
            return oXmlElement;
        }

        CXmlElement oCharXmlElement;

        oXmlElement.CreateDefaultElement(sNameStyle);

        if (sNameStyle[0] == L'h' && isdigit(sNameStyle[1]) && sNameStyle.length() == 2)
        {
            std::wstring sCharName = L"title";
            sCharName += sNameStyle[1];
            sCharName += L"-c";

            if (std::find(m_arStandardStyles.begin(), m_arStandardStyles.end(), sCharName) != m_arStandardStyles.cend())
            {
                oCharXmlElement.CreateDefaultElement(sCharName);
                m_arStandardStyles.erase(std::remove(m_arStandardStyles.begin(), m_arStandardStyles.end(), sCharName), m_arStandardStyles.end());
            }
        }
        else if (sNameStyle == L"p" && std::find(m_arStandardStyles.begin(), m_arStandardStyles.end(), L"p-c") != m_arStandardStyles.cend())
        {
            oCharXmlElement.CreateDefaultElement(L"p-c");
            m_arStandardStyles.erase(std::remove(m_arStandardStyles.begin(), m_arStandardStyles.end(), L"p-c"), m_arStandardStyles.end());
        }
        else if (sNameStyle == L"div" && std::find(m_arStandardStyles.begin(), m_arStandardStyles.end(), L"div-c") != m_arStandardStyles.cend())
        {
            oCharXmlElement.CreateDefaultElement(L"div-c");
            m_arStandardStyles.erase(std::remove(m_arStandardStyles.begin(), m_arStandardStyles.end(), L"div-c"), m_arStandardStyles.end());
        }
        else if (sNameStyle == L"a" && std::find(m_arStandardStyles.begin(), m_arStandardStyles.end(), L"a-c") != m_arStandardStyles.cend())
        {
            oCharXmlElement.CreateDefaultElement(L"a-c");
            m_arStandardStyles.erase(std::remove(m_arStandardStyles.begin(), m_arStandardStyles.end(), L"a-c"), m_arStandardStyles.end());
        }
        if (!oCharXmlElement.Empty())
        {
            m_sStyle += oCharXmlElement.GetRStyle();
        }

        return oXmlElement;
    }

    CXmlElement CDocumentStyle::ConvertStyle(const NSCSS::CCompiledStyle &oStyle, bool bIsPStyle)
    {
        std::wstring sName = oStyle.GetId();

        if (sName.find(L'-') != std::wstring::npos)
            sName = sName.substr(0, sName.find(L'-'));
        if (sName.find(L'#') != std::wstring::npos)
            sName = sName.substr(0, sName.find(L'#'));
        if (sName.find(L'.') != std::wstring::npos)
            sName = sName.substr(0, sName.find(L'.'));

        CXmlElement oXmlElement;
        CXmlElement oStandardXmlElement;
        CXmlElement oParentStyle;

        if (std::find(m_arStandardStyles.begin(), m_arStandardStyles.end(), sName) != m_arStandardStyles.cend())
        {
            oStandardXmlElement = CreateStandardStyle(sName);

            if (!bIsPStyle && !oStandardXmlElement.Empty())
            {
                oStandardXmlElement.SetStyleId(oStandardXmlElement.GetStyleId() + L"-c");
                oStandardXmlElement.SetType(L"character");
            }
        }

        if (oStyle.GetParentsName().size() > 0)
        {
            std::vector<std::wstring> arParentsName = oStyle.GetParentsName();

            std::wstring sParentName;
            for (std::wstring sParent : arParentsName)
            {
                sParentName += sParent;
                if (sParent != arParentsName[arParentsName.size() - 1])
                {
                    sParentName += L"+";
                }
            }
            oParentStyle = CombineStandardStyles(arParentsName);

            if (!oParentStyle.Empty())
            {
                oParentStyle.SetStyleId(L"(" + sParentName + L")");
                if (!bIsPStyle)
                {
                    oParentStyle.SetStyleId(oParentStyle.GetStyleId() + L"-c");
                    oParentStyle.SetType(L"character");
                }
            }
        }

        if (!oStandardXmlElement.Empty() && !oParentStyle.Empty())
        {
            if (std::find(m_arStandardStylesUsed.begin(), m_arStandardStylesUsed.end(), oStandardXmlElement.GetStyleId() + oParentStyle.GetStyleId() ) != m_arStandardStylesUsed.cend())
            {
                oXmlElement.SetBasedOn(oStandardXmlElement.GetStyleId() + oParentStyle.GetStyleId());
            }
            else
            {
                CXmlElement oTempXmlElement;
                oTempXmlElement = oParentStyle;
                oTempXmlElement += oStandardXmlElement;
                if (!oTempXmlElement.Empty())
                {
                    oTempXmlElement.SetStyleId(oStandardXmlElement.GetStyleId() + oParentStyle.GetStyleId());

                    if (bIsPStyle)
                        m_sStyle = oTempXmlElement.GetPStyle();
                    else
                        m_sStyle += oTempXmlElement.GetRStyle();

                    oXmlElement.SetBasedOn(oTempXmlElement.GetStyleId());
                }
            }
        }
        else if (!oStandardXmlElement.Empty() && oParentStyle.Empty())
        {
            if (std::find(m_arStandardStylesUsed.begin(), m_arStandardStylesUsed.end(), oStandardXmlElement.GetStyleId()) != m_arStandardStylesUsed.cend())
            {
                oXmlElement.SetBasedOn(oStandardXmlElement.GetStyleId());
            }
            else
            {
                if (bIsPStyle)
                    m_sStyle = oStandardXmlElement.GetPStyle();
                else
                    m_sStyle += oStandardXmlElement.GetRStyle();

                m_arStandardStylesUsed.push_back(oStandardXmlElement.GetStyleId());

                oXmlElement.SetBasedOn(oStandardXmlElement.GetStyleId());
            }
        }
        else if (oStandardXmlElement.Empty() && !oParentStyle.Empty())
        {
            if (std::find(m_arStandardStylesUsed.begin(), m_arStandardStylesUsed.end(), oParentStyle.GetStyleId()) == m_arStandardStylesUsed.cend())
            {
                if (std::find(m_arStandardStyles.begin(), m_arStandardStyles.end(), oParentStyle.GetName()) != m_arStandardStyles.cend())
                {
                    oXmlElement.SetBasedOn(oParentStyle.GetName());
                }
                else
                {
                    m_arStandardStylesUsed.push_back(oParentStyle.GetStyleId());
//                    std::sort(m_arStandardStylesUsed.begin(), m_arStandardStylesUsed.end());
                    m_sStyle = oParentStyle.GetStyle();
                    oXmlElement.SetBasedOn(oParentStyle.GetStyleId());
                }
            }
            else
                oXmlElement.SetBasedOn(oParentStyle.GetStyleId());
        }

        if (oStyle.Empty())
        {
            if (oXmlElement.GetBasedOn().empty() || (oStandardXmlElement.Empty() && oParentStyle.Empty()))
                m_sId = L"normal";
            else
                m_sId = oXmlElement.GetBasedOn();

            oXmlElement.Clear();
            return oXmlElement;
        }

        m_sId = oStyle.GetId();
        oXmlElement.SetStyleId(m_sId);
        oXmlElement.SetName(m_sId);
        oXmlElement.SetQFormat(true);

        if (bIsPStyle)
            oXmlElement.SetType(L"paragraph");
        else
            oXmlElement.SetType(L"character");

        oXmlElement.SetCustomStyle(L"1");

        return oXmlElement;

    }

    void CDocumentStyle::SetPStyle(NSCSS::CCompiledStyle &oStyle, CXmlElement& oXmlElement)
    {
        oXmlElement = ConvertStyle(oStyle, true);

        if (oStyle.Empty())
            return;

        if (!oStyle.GetTextAlign().empty())
        {
            std::wstring sTextAlign = oStyle.GetTextAlign();
            oXmlElement.SetJc(oStyle.GetTextAlign());
        }

        std::wstring sIndValue;
        if (!oStyle.GetTextIndent().empty() && oStyle.GetId().find(L"table") == std::wstring::npos)
        {
            sIndValue += L"w:firstLine=\"" + oStyle.GetTextIndent() + L"\" ";
        }
        if (!oStyle.GetMargin().empty())
        {
            double dLeftValue = wcstod(oStyle.GetMarginLeft().c_str(), NULL);
            dLeftValue = dLeftValue * 1440 / 72;

            double dRightValue = wcstod(oStyle.GetMarginRight().c_str(), NULL);
            dRightValue = dRightValue * 1440 / 72;

            sIndValue += L"w:left=\"" + std::to_wstring((int)floor(dLeftValue + 0.5)) + L"\" ";
            sIndValue += L"w:right=\"" + std::to_wstring((int)floor(dRightValue + 0.5)) + L"\"";
        }

        if (!sIndValue.empty())
        {
            oXmlElement.SetInd(sIndValue);
        }

        std::wstring sSpacingValue;

        if (!oStyle.GetMargin().empty())
        {
            double dAfterValue = wcstod(oStyle.GetMarginTop().c_str(), NULL);
            dAfterValue *= 10;

            double dBeforeValue = wcstod(oStyle.GetMarginBottom().c_str(), NULL);
            dBeforeValue *= 10;

            sSpacingValue += L"w:after=\"" + std::to_wstring((int)floor(dAfterValue + 0.5)) + L"\" ";
            sSpacingValue += L"w:before=\"" + std::to_wstring((int)floor(dBeforeValue + 0.5)) + L"\" ";
        }

        if (!oStyle.GetLineHeight().empty())
        {
            double dLineHeight = wcstod(oStyle.GetLineHeight().c_str(), NULL);
            dLineHeight *= 11;
            sSpacingValue += L"w:line=\"" + std::to_wstring((int)floor(dLineHeight + 0.5)) + L"\" ";
            sSpacingValue += L"w:lineRule=\"auto\"";
        }

        if (!sSpacingValue.empty())
        {
            oXmlElement.SetSpacing(sSpacingValue);
            oXmlElement.SetContextualSpacing(true);
        }

        if (!oStyle.GetBackgroundColor().empty())
        {
            oXmlElement.SetShd(oStyle.GetBackgroundColor());
        }

        if (!oStyle.GetBorder().empty())
        {
            oXmlElement.SetTopBorder(L"true");
            oXmlElement.SetLeftBorder(L"true");
            oXmlElement.SetBottomBorder(L"true");
            oXmlElement.SetRightBorder(L"true");
        }
    }

    void CDocumentStyle::SetRStyle(NSCSS::CCompiledStyle &oStyle, CXmlElement& oXmlElement)
    {
        oXmlElement = ConvertStyle(oStyle, false);

        if (oStyle.Empty())
            return;

        if (!oStyle.GetColor().empty())
            oXmlElement.SetColor(oStyle.GetColor());

        if (!oStyle.GetFontSize().empty())
        {
            std::wstring sFontSize = oStyle.GetFontSize();

            double dValue = wcstod(sFontSize.c_str(), NULL);
            dValue *= 2;
            oXmlElement.SetSz(std::to_wstring((int)dValue));
        }
        if (!oStyle.GetFontFamily().empty())
        {
            std::wstring sFontFamily = oStyle.GetFontFamily();
            oXmlElement.SetRFonts(sFontFamily);
        }

        if (!oStyle.GetFontWeight().empty())
        {
            std::wstring sFontWeight = oStyle.GetFontWeight();
            if (sFontWeight == L"bold")
                oXmlElement.SetB(true);
        }

        if (!oStyle.GetFontStyle().empty())
        {
            std::wstring sFontStyle = oStyle.GetFontStyle();
            if (sFontStyle == L"italic")
            {
                oXmlElement.SetI(true);
            }
        }

        if (!oStyle.GetBorder().empty() && oStyle.GetId().find(L"caption") == std::wstring::npos)
        {
            oXmlElement.SetTopBorder(L"true");
            oXmlElement.SetLeftBorder(L"true");
            oXmlElement.SetBottomBorder(L"true");
            oXmlElement.SetRightBorder(L"true");
        }

        std::wstring sSpacingValue;

        if (!oStyle.GetMargin().empty())
        {
            double dAfterValue = wcstod(oStyle.GetMarginTop().c_str(), NULL);
            dAfterValue *= 10;

            double dBeforeValue = wcstod(oStyle.GetMarginBottom().c_str(), NULL);
            dBeforeValue *= 10;

            sSpacingValue += L"w:after=\"" + std::to_wstring((int)floor(dAfterValue + 0.5)) + L"\" ";
            sSpacingValue += L"w:before=\"" + std::to_wstring((int)floor(dBeforeValue + 0.5)) + L"\" ";
        }

        if (!oStyle.GetLineHeight().empty())
        {
            double dLineHeight = wcstod(oStyle.GetLineHeight().c_str(), NULL);
            dLineHeight *= 22;
            sSpacingValue += L"w:line=\"" + std::to_wstring((int)floor(dLineHeight + 0.5)) + L"\" ";
            sSpacingValue += L"w:lineRule=\"auto\"";
        }

        if (!sSpacingValue.empty())
        {
            oXmlElement.SetSpacing(sSpacingValue);
            oXmlElement.SetContextualSpacing(true);
        }

        if (!oStyle.GetColor().empty())
        {
            oXmlElement.SetColor(oStyle.GetColor());
        }
    }

    void CDocumentStyle::WriteRStyle(NSCSS::CCompiledStyle &oStyle)
    {
        if(oStyle.GetId().empty())
        {
            m_sId = L"normal";
            m_sStyle.clear();
            return;
        }

        for (auto oItem : m_arStyleUsed)
        {
            if(oItem.first == oStyle && !oItem.second)
            {
                m_sId = oItem.first.GetId();
                m_sStyle.clear();
                return;
            }
        }

        CXmlElement oXmlElement;
        SetRStyle(oStyle, oXmlElement);

        if (!oStyle.Empty())
        {
            m_arStyleUsed.push_back(std::make_pair(oStyle, false));

//            std::sort(m_arStyleUsed.begin(), m_arStyleUsed.end(), Comp);
        }
        if (!oXmlElement.Empty())
            m_sStyle += oXmlElement.GetRStyle();
    }

    void CDocumentStyle::WritePStyle(NSCSS::CCompiledStyle &oStyle)
    {
        if(oStyle.GetId().empty())
        {
            m_sId = L"normal";
            m_sStyle.clear();
            return;
        }
        for (auto oItem : m_arStyleUsed)
        {
            if(oItem.first == oStyle && oItem.second)
            {
                m_sId = oItem.first.GetId();
                m_sStyle.clear();
                return;
            }
        }

        CXmlElement oXmlElement;
        SetPStyle(oStyle, oXmlElement);

        if (!oStyle.Empty())
        {
            m_arStyleUsed.push_back(std::make_pair(oStyle, true));

//            std::sort(m_arStyleUsed.begin(), m_arStyleUsed.end(), Comp);
        }
        if (!oXmlElement.Empty())
            m_sStyle += oXmlElement.GetPStyle();
    }
}
