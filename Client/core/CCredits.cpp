/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CCredits.cpp
 *  PURPOSE:     In-game credits window implementation
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

namespace CCreditsGUI
{
    float fWindowX = 560.0f;
    float fWindowY = 300.0f;
}            // namespace CCreditsGUI
using namespace CCreditsGUI;

CCredits::CCredits()
{
    CGUI* pManager = g_pCore->GetGUI();

    m_strCredits += "== Project Monky"
                    " ==\n"
                    "\n";

    m_strCredits += _("Founder");
    m_strCredits +=
        "\n\n"
        "5150\n"
        "\n\n";

    m_strCredits += _("Manager");
    m_strCredits +=
        "\n\n"
        "Leon\n"
        "\n\n";

    m_strCredits += _("Project Monky Devs and Contributors");
    m_strCredits +=
        "\n\n"
        "Staz - Client creation\n"
        "Portside - Initial website creation\n"
        "Leon - Major contributions to in-game work\n"
        "Loki - Contributions to code\n"
        "lbdBG - Initial VPS curator and security management (Retired)\n"
        "\n\n";

    m_strCredits += _("Special Thanks To...");
        m_strCredits +=
            "\n\n"
            "Everyone following and supporting us by playing\n"
            "All testers\n"
            "AGRP\n"
            "Multi Theft Auto for banning me and pushing me to create this";

    // Create our window
    CVector2D RelativeWindow = CVector2D(fWindowX / pManager->GetResolution().fX, fWindowY / pManager->GetResolution().fY);
    m_pWindow = reinterpret_cast<CGUIWindow*>(pManager->CreateWnd(NULL, "Project Monky"));
    m_pWindow->SetCloseButtonEnabled(false);
    m_pWindow->SetMovable(false);
    m_pWindow->SetPosition(CVector2D(0.5f - RelativeWindow.fX * 0.5f, 0.5f - RelativeWindow.fY * 0.5f), true);
    m_pWindow->SetSize(CVector2D(fWindowX, fWindowY));            // relative 0.70, 0.50
    m_pWindow->SetSizingEnabled(false);
    m_pWindow->SetVisible(false);
    m_pWindow->SetAlwaysOnTop(true);

    // Credits label
    memset(m_pLabels, 0, sizeof(CGUILabel*) * MAX_CREDITS_LABEL_COUNT);

    // Create one for every 15th line. This is because of some limit at 500 chars
    float        fStartPosition = 1.0f;
    const char*  szCreditsIterator = m_strCredits.c_str();
    const char*  szCreditsBegin = m_strCredits.c_str();
    unsigned int uiLineCount = 0;
    unsigned int uiLabelIndex = 0;
    while (true)
    {
        // Count every new line
        if (*szCreditsIterator == '\n')
            ++uiLineCount;

        // 15? Create a new label
        if (uiLineCount >= 15 || *szCreditsIterator == 0)
        {
            // Copy out the text we shall put in that label
            std::string strBuffer(szCreditsBegin, szCreditsIterator - szCreditsBegin);

            // Remember where we count from
            szCreditsBegin = szCreditsIterator;
            ++szCreditsBegin;

            // Create the label
            m_pLabels[uiLabelIndex] = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, strBuffer.c_str()));
            m_pLabels[uiLabelIndex]->SetPosition(CVector2D(0.022f, fStartPosition), true);
            m_pLabels[uiLabelIndex]->SetSize(CVector2D(532.0f, 1500.0f));            // relative 0.95, 6.0
            m_pLabels[uiLabelIndex]->SetHorizontalAlign(CGUI_ALIGN_HORIZONTALCENTER);
            ++uiLabelIndex;

            // Reset the linecount
            uiLineCount = 0;
        }

        // End the loop at 0
        if (*szCreditsIterator == 0)
            break;

        // Increase the credits iterator
        ++szCreditsIterator;
    }

    // Create the OK button
    m_pButtonOK = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pWindow, "OK"));
    m_pButtonOK->SetPosition(CVector2D(0.77f, 0.90f), true);
    m_pButtonOK->SetSize(CVector2D(112.0f, 21.0f));            // relative 0.20, 0.07
    m_pButtonOK->SetVisible(true);
    m_pButtonOK->SetAlwaysOnTop(true);

    // Set up the event handlers
    m_pButtonOK->SetClickHandler(GUI_CALLBACK(&CCredits::OnOKButtonClick, this));
    m_pWindow->SetEnterKeyHandler(GUI_CALLBACK(&CCredits::OnOKButtonClick, this));
}

CCredits::~CCredits()
{
    // Delete all the labels
    int i = 0;
    for (; i < MAX_CREDITS_LABEL_COUNT; i++)
    {
        if (m_pLabels[i])
        {
            delete m_pLabels[i];
            m_pLabels[i] = NULL;
        }
    }

    // Delete the controls
    delete m_pButtonOK;

    // Delete our window
    delete m_pWindow;
}

void CCredits::Update()
{
    if (m_pWindow->IsVisible())
    {
        // Speed it up if arrow keys are being held
        if ((GetAsyncKeyState(VK_DOWN) & 0x8000) != 0)
        {
            m_clkStart -= 150;
        }
        if ((GetAsyncKeyState(VK_UP) & 0x8000) != 0)
        {
            m_clkStart += 150;
        }

        // Calculate the current position
        float fCurrentTop = 1.0f - 0.0001f * static_cast<float>(clock() - m_clkStart);

        // If we're too far down or up, make sure we restart next pulse
        if (fCurrentTop <= -63.0f || fCurrentTop >= 1.0f)
        {
            m_clkStart = clock();
        }

        // Move everything by the time
        int i = 0;
        for (; i < MAX_CREDITS_LABEL_COUNT; i++)
        {
            if (m_pLabels[i])
            {
                m_pLabels[i]->SetPosition(CVector2D(0.022f, fCurrentTop + i * 0.725f), true);
            }
        }
    }
}

void CCredits::SetVisible(bool bVisible)
{
    m_pWindow->SetVisible(bVisible);
    m_pWindow->BringToFront();
    m_clkStart = clock();
}

bool CCredits::IsVisible()
{
    return m_pWindow->IsVisible();
}

bool CCredits::OnOKButtonClick(CGUIElement* pElement)
{
    CMainMenu* pMainMenu = CLocalGUI::GetSingleton().GetMainMenu();

    // Close the window
    m_pWindow->SetVisible(false);
    pMainMenu->m_bIsInSubWindow = false;

    return true;
}
