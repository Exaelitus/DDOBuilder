// SpecialFeatsView.cpp
//
#include "stdafx.h"
#include "SpecialFeatsView.h"

#include "DDOCP.h"
#include "FeatSelectionDialog.h"
#include "GlobalSupportFunctions.h"

namespace
{
    const int c_controlSpacing = 1;
    const int c_windowSizeX = 39;
    const int c_windowSizeY = 55;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
}

IMPLEMENT_DYNCREATE(CSpecialFeatsView, CFormView)

CSpecialFeatsView::CSpecialFeatsView() :
    CFormView(CSpecialFeatsView::IDD)
{
    // fixed sized font in use for static controls
    LOGFONT lf;
    ZeroMemory((PVOID)&lf, sizeof(LOGFONT));
    NONCLIENTMETRICS nm;
    nm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0));
    lf = nm.lfMenuFont;
    lf.lfHeight = -12;
    m_staticFont.CreateFontIndirect(&lf);
}

CSpecialFeatsView::~CSpecialFeatsView()
{
    for (size_t vi = 0; vi < m_heroicSelectionViews.size(); ++vi)
    {
        delete m_heroicSelectionViews[vi];
    }
    for (size_t vi = 0; vi < m_racialSelectionViews.size(); ++vi)
    {
        delete m_racialSelectionViews[vi];
    }
    for (size_t vi = 0; vi < m_iconicSelectionViews.size(); ++vi)
    {
        delete m_iconicSelectionViews[vi];
    }
    for (size_t vi = 0; vi < m_epicSelectionViews.size(); ++vi)
    {
        delete m_epicSelectionViews[vi];
    }
    for (size_t vi = 0; vi < m_specialSelectionViews.size(); ++vi)
    {
        delete m_specialSelectionViews[vi];
    }
    for (size_t vi = 0; vi < m_favorSelectionViews.size(); ++vi)
    {
        delete m_favorSelectionViews[vi];
    }
}

void CSpecialFeatsView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    // DDX the number of controls that we have feats for
    DDX_Control(pDX, IDC_STATIC_HEROIC, m_staticHeroic);
    DDX_Control(pDX, IDC_STATIC_RACIAL, m_staticRacial);
    DDX_Control(pDX, IDC_STATIC_ICONIC, m_staticIconic);
    DDX_Control(pDX, IDC_STATIC_EPIC, m_staticEpic);
    DDX_Control(pDX, IDC_STATIC_SPECIAL, m_staticSpecial);
    DDX_Control(pDX, IDC_STATIC_FAVOR, m_staticFavor);

    m_staticHeroic.SetFont(&m_staticFont);
    m_staticRacial.SetFont(&m_staticFont);
    m_staticIconic.SetFont(&m_staticFont);
    m_staticEpic.SetFont(&m_staticFont);
    m_staticSpecial.SetFont(&m_staticFont);
    m_staticFavor.SetFont(&m_staticFont);

    // create the dynamic controls used to add feats
    CWinApp * pApp = AfxGetApp();
    CDDOCPApp * pDDOApp = dynamic_cast<CDDOCPApp*>(pApp);
    if (pDDOApp != NULL)
    {
        const std::list<Feat> & heroicPastLifeFeats = pDDOApp->HeroicPastLifeFeats();
        const std::list<Feat> & racialPastLifeFeats = pDDOApp->RacialPastLifeFeats();
        const std::list<Feat> & iconicPastLifeFeats = pDDOApp->IconicPastLifeFeats();
        const std::list<Feat> & epicPastLifeFeats = pDDOApp->EpicPastLifeFeats();
        const std::list<Feat> & specialFeats = pDDOApp->SpecialFeats();
        const std::list<Feat> & favorFeats = pDDOApp->FavorFeats();
        m_heroicSelectionViews.reserve(heroicPastLifeFeats.size());
        m_racialSelectionViews.reserve(racialPastLifeFeats.size());
        m_iconicSelectionViews.reserve(iconicPastLifeFeats.size());
        m_epicSelectionViews.reserve(epicPastLifeFeats.size());
        m_specialSelectionViews.reserve(specialFeats.size());
        m_favorSelectionViews.reserve(favorFeats.size());

        CreateFeatWindows(&m_staticHeroic, heroicPastLifeFeats, &m_heroicSelectionViews, TFT_HeroicPastLife);
        CreateFeatWindows(&m_staticRacial, racialPastLifeFeats, &m_racialSelectionViews, TFT_RacialPastLife);
        CreateFeatWindows(&m_staticIconic, iconicPastLifeFeats, &m_iconicSelectionViews, TFT_IconicPastLife);
        CreateFeatWindows(&m_staticEpic, epicPastLifeFeats, &m_epicSelectionViews, TFT_EpicPastLife);
        CreateFeatWindows(&m_staticSpecial, specialFeats, &m_specialSelectionViews, TFT_SpecialFeat);
        CreateFeatWindows(&m_staticFavor, favorFeats, &m_favorSelectionViews, TFT_FavorFeat);
    }
}

void CSpecialFeatsView::CreateFeatWindows(
        CStatic * groupWindow,
        const std::list<Feat> & featList,
        std::vector<CDialog *> * dialogs,
        TrainableFeatTypes type)
{
    // position the created windows left to right under the static control until
    // they don't fit then move them down a row and start again
    CRect wndRect;
    groupWindow->GetWindowRect(wndRect);
    ScreenToClient(&wndRect);

    CRect itemRect(
            c_controlSpacing,
            c_controlSpacing,
            c_windowSizeX + c_controlSpacing,
            c_windowSizeY + c_controlSpacing);
    itemRect += CPoint(0, wndRect.bottom);
    std::list<Feat>::const_iterator li = featList.begin();
    for (size_t fi = 0; fi < featList.size(); ++fi)
    {
        // show a feat selection dialog
        CFeatSelectionDialog * dlg = new CFeatSelectionDialog(this, *li, type);
        dlg->Create(CFeatSelectionDialog::IDD, this);
        dlg->MoveWindow(&itemRect);
        dlg->ShowWindow(SW_SHOW);
        dialogs->push_back(dlg);
        li++;
        // move rectangle across for next set of controls
        itemRect += CPoint(itemRect.Width() + c_controlSpacing, 0);
        if (itemRect.right > (wndRect.right - c_controlSpacing))
        {
            // oops, not enough space in client area here
            // move down and start the next row of controls
            itemRect -= CPoint(itemRect.left, 0);
            itemRect += CPoint(c_controlSpacing, itemRect.Height() + c_controlSpacing);
        }
    }
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CSpecialFeatsView, CFormView)
    ON_WM_SIZE()
    //ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
END_MESSAGE_MAP()
#pragma warning(pop)

#ifdef _DEBUG
void CSpecialFeatsView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSpecialFeatsView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CSpecialFeatsView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
}

void CSpecialFeatsView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);
    // move all our feat windows to be as many across as we can, then start the next row
    if (IsWindow(m_staticHeroic.GetSafeHwnd()))
    {
        // position the created windows left to right until they don't fit
        // then move them down a row and start again
        // do this for each set of controls
        size_t fi = 0;
        int maxx = c_controlSpacing;
        int maxy = c_controlSpacing;
        fi = PositionWindows(&m_staticHeroic, fi, m_heroicSelectionViews, &maxx, &maxy);
        fi = PositionWindows(&m_staticRacial, fi, m_racialSelectionViews, &maxx, &maxy);
        fi = PositionWindows(&m_staticIconic, fi, m_iconicSelectionViews, &maxx, &maxy);
        fi = PositionWindows(&m_staticEpic, fi, m_epicSelectionViews, &maxx, &maxy);
        fi = PositionWindows(&m_staticSpecial, fi, m_specialSelectionViews, &maxx, &maxy);
        fi = PositionWindows(&m_staticFavor, fi, m_favorSelectionViews, &maxx, &maxy);
        // set scale based on area used by the windows.
        // This will introduce scroll bars if required
        SetScrollSizes(MM_TEXT, CSize(maxx, maxy));
    }
}

size_t CSpecialFeatsView::PositionWindows(
        CStatic * groupWindow,
        size_t startIndex,
        const std::vector<CDialog *> & dialogs,
        int * maxX,
        int * yPos)
{
    // first position the group control
    CRect rctGroup;
    groupWindow->GetWindowRect(&rctGroup);
    rctGroup -= rctGroup.TopLeft();
    rctGroup.bottom = rctGroup.top + c_windowSizeY;
    rctGroup += CPoint(c_controlSpacing, *yPos);
    // position the group control
    groupWindow->MoveWindow(rctGroup);

    // now position the group windows immediately to the right
    CRect itemRect(c_controlSpacing, *yPos, c_windowSizeX, c_windowSizeY + *yPos);
    itemRect += CPoint(rctGroup.Width() + c_controlSpacing, 0);
    for (size_t fi = 0; fi < dialogs.size(); ++fi)
    {
        dialogs[fi]->MoveWindow(itemRect);
        // move rectangle across for next set of controls
        itemRect += CPoint(itemRect.Width() + c_controlSpacing, 0);
    }
    // always 1 line of icons
    *yPos += itemRect.Height() + c_controlSpacing;
    // keep track of the longest line of icons
    *maxX = max(*maxX, itemRect.left);
    return startIndex + dialogs.size();
}

LRESULT CSpecialFeatsView::OnNewDocument(WPARAM wParam, LPARAM lParam)
{
    // wParam is the document pointer
    CDocument * pDoc = (CDocument*)(wParam);
    // lParam is the character pointer
    Character * pCharacter = (Character *)(lParam);
    if (pDoc != m_pDocument)
    {
        //::OutputDebugString("CSpecialFeatsView::OnNewDocument\n");
        m_pDocument = pDoc;
        for (size_t vi = 0; vi < m_heroicSelectionViews.size(); ++vi)
        {
            if (IsWindow(m_heroicSelectionViews[vi]->GetSafeHwnd()))
            {
                m_heroicSelectionViews[vi]->SendMessage(UWM_NEW_DOCUMENT, (WPARAM)m_pDocument, (LPARAM)pCharacter);
            }
        }
        for (size_t vi = 0; vi < m_racialSelectionViews.size(); ++vi)
        {
            if (IsWindow(m_racialSelectionViews[vi]->GetSafeHwnd()))
            {
                m_racialSelectionViews[vi]->SendMessage(UWM_NEW_DOCUMENT, (WPARAM)m_pDocument, (LPARAM)pCharacter);
            }
        }
        for (size_t vi = 0; vi < m_iconicSelectionViews.size(); ++vi)
        {
            if (IsWindow(m_iconicSelectionViews[vi]->GetSafeHwnd()))
            {
                m_iconicSelectionViews[vi]->SendMessage(UWM_NEW_DOCUMENT, (WPARAM)m_pDocument, (LPARAM)pCharacter);
            }
        }
        for (size_t vi = 0; vi < m_epicSelectionViews.size(); ++vi)
        {
            if (IsWindow(m_epicSelectionViews[vi]->GetSafeHwnd()))
            {
                m_epicSelectionViews[vi]->SendMessage(UWM_NEW_DOCUMENT, (WPARAM)m_pDocument, (LPARAM)pCharacter);
            }
        }
        for (size_t vi = 0; vi < m_specialSelectionViews.size(); ++vi)
        {
            if (IsWindow(m_specialSelectionViews[vi]->GetSafeHwnd()))
            {
                m_specialSelectionViews[vi]->SendMessage(UWM_NEW_DOCUMENT, (WPARAM)m_pDocument, (LPARAM)pCharacter);
            }
        }
        for (size_t vi = 0; vi < m_favorSelectionViews.size(); ++vi)
        {
            if (IsWindow(m_favorSelectionViews[vi]->GetSafeHwnd()))
            {
                m_favorSelectionViews[vi]->SendMessage(UWM_NEW_DOCUMENT, (WPARAM)m_pDocument, (LPARAM)pCharacter);
            }
        }
    }
    return 0L;
}

BOOL CSpecialFeatsView::OnEraseBkgnd(CDC* pDC)
{
    //::OutputDebugString("CSpecialFeatsView::OnEraseBkgnd\n");
    static int controlsNotToBeErased[] =
    {
        IDC_STATIC_HEROIC,
        IDC_STATIC_RACIAL,
        IDC_STATIC_ICONIC,
        IDC_STATIC_EPIC,
        IDC_STATIC_SPECIAL,
        IDC_STATIC_FAVOR,
        0 // end marker
    };

    BOOL ret = OnEraseBackground(this, pDC, controlsNotToBeErased);
    
    // now handle the dynamic controls also
    for (size_t i = 0; i < m_heroicSelectionViews.size(); ++i)
    {
        CWnd * pControl = m_heroicSelectionViews[i];
        if (pControl && pControl->IsWindowVisible())
        {
            CRect controlClip;
            pControl->GetWindowRect(&controlClip);
            ScreenToClient(&controlClip);
            if (pControl->IsKindOf(RUNTIME_CLASS(CComboBox)))
            {
                // combo boxes return the height of the whole control, including the drop rectangle
                // limit to the the height of the selection combo
                controlClip.bottom = controlClip.top
                        + GetSystemMetrics(SM_CYHSCROLL)
                        + GetSystemMetrics(SM_CYEDGE) * 2;
            }
            pDC->ExcludeClipRect(&controlClip);
        }
    }
    for (size_t i = 0; i < m_racialSelectionViews.size(); ++i)
    {
        CWnd * pControl = m_racialSelectionViews[i];
        if (pControl && pControl->IsWindowVisible())
        {
            CRect controlClip;
            pControl->GetWindowRect(&controlClip);
            ScreenToClient(&controlClip);
            if (pControl->IsKindOf(RUNTIME_CLASS(CComboBox)))
            {
                // combo boxes return the height of the whole control, including the drop rectangle
                // limit to the the height of the selection combo
                controlClip.bottom = controlClip.top
                        + GetSystemMetrics(SM_CYHSCROLL)
                        + GetSystemMetrics(SM_CYEDGE) * 2;
            }
            pDC->ExcludeClipRect(&controlClip);
        }
    }
    for (size_t i = 0; i < m_iconicSelectionViews.size(); ++i)
    {
        CWnd * pControl = m_iconicSelectionViews[i];
        if (pControl && pControl->IsWindowVisible())
        {
            CRect controlClip;
            pControl->GetWindowRect(&controlClip);
            ScreenToClient(&controlClip);
            if (pControl->IsKindOf(RUNTIME_CLASS(CComboBox)))
            {
                // combo boxes return the height of the whole control, including the drop rectangle
                // limit to the the height of the selection combo
                controlClip.bottom = controlClip.top
                        + GetSystemMetrics(SM_CYHSCROLL)
                        + GetSystemMetrics(SM_CYEDGE) * 2;
            }
            pDC->ExcludeClipRect(&controlClip);
        }
    }
    for (size_t i = 0; i < m_epicSelectionViews.size(); ++i)
    {
        CWnd * pControl = m_epicSelectionViews[i];
        if (pControl && pControl->IsWindowVisible())
        {
            CRect controlClip;
            pControl->GetWindowRect(&controlClip);
            ScreenToClient(&controlClip);
            if (pControl->IsKindOf(RUNTIME_CLASS(CComboBox)))
            {
                // combo boxes return the height of the whole control, including the drop rectangle
                // limit to the the height of the selection combo
                controlClip.bottom = controlClip.top
                        + GetSystemMetrics(SM_CYHSCROLL)
                        + GetSystemMetrics(SM_CYEDGE) * 2;
            }
            pDC->ExcludeClipRect(&controlClip);
        }
    }
    for (size_t i = 0; i < m_specialSelectionViews.size(); ++i)
    {
        CWnd * pControl = m_specialSelectionViews[i];
        if (pControl && pControl->IsWindowVisible())
        {
            CRect controlClip;
            pControl->GetWindowRect(&controlClip);
            ScreenToClient(&controlClip);
            if (pControl->IsKindOf(RUNTIME_CLASS(CComboBox)))
            {
                // combo boxes return the height of the whole control, including the drop rectangle
                // limit to the the height of the selection combo
                controlClip.bottom = controlClip.top
                        + GetSystemMetrics(SM_CYHSCROLL)
                        + GetSystemMetrics(SM_CYEDGE) * 2;
            }
            pDC->ExcludeClipRect(&controlClip);
        }
    }
    for (size_t i = 0; i < m_favorSelectionViews.size(); ++i)
    {
        CWnd * pControl = m_favorSelectionViews[i];
        if (pControl && pControl->IsWindowVisible())
        {
            CRect controlClip;
            pControl->GetWindowRect(&controlClip);
            ScreenToClient(&controlClip);
            if (pControl->IsKindOf(RUNTIME_CLASS(CComboBox)))
            {
                // combo boxes return the height of the whole control, including the drop rectangle
                // limit to the the height of the selection combo
                controlClip.bottom = controlClip.top
                        + GetSystemMetrics(SM_CYHSCROLL)
                        + GetSystemMetrics(SM_CYEDGE) * 2;
            }
            pDC->ExcludeClipRect(&controlClip);
        }
    }

    return ret;
}
