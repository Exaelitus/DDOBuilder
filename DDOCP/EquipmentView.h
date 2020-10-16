// EquipmentView.h
//
#pragma once
#include "Resource.h"
#include "Character.h"
#include "InventoryDialog.h"

class CEquipmentView :
    public CFormView,
    public InventoryObserver,
    public CharacterObserver
{
    public:
        enum { IDD = IDD_EQUIPMENT_VIEW };
        DECLARE_DYNCREATE(CEquipmentView)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void OnInitialUpdate();
    protected:
        CEquipmentView();           // protected constructor used by dynamic creation
        virtual ~CEquipmentView();

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg void OnGearNew();
        afx_msg void OnGearCopy();
        afx_msg void OnGearPaste();
        afx_msg void OnGearDelete();
        afx_msg void OnUpdateGearNew(CCmdUI * pCmdUi);
        afx_msg void OnUpdateGearCopy(CCmdUI * pCmdUi);
        afx_msg void OnUpdateGearPaste(CCmdUI * pCmdUi);
        afx_msg void OnUpdateGearDelete(CCmdUI * pCmdUi);
        afx_msg void OnGearSelectionSelEndOk();
        afx_msg void OnGearNumFiligreesSelEndOk();
        afx_msg void OnToggleFiligreeMenu();
        DECLARE_MESSAGE_MAP()

        // InventoryObserver overrides
        virtual void UpdateSlotLeftClicked(CInventoryDialog * dialog, InventorySlotType slot) override;
        virtual void UpdateSlotRightClicked(CInventoryDialog * dialog, InventorySlotType slot) override;

        // character overrides
        virtual void UpdateGearChanged(Character * charData, InventorySlotType slot) override;

    private:
        void PopulateCombobox();
        void EnableControls();
        void PopulateGear();
        std::string SelectedGearSet() const;
        CComboBox m_comboGearSelections;
        CMFCButton m_buttonNew;
        CMFCButton m_buttonCopy;
        CMFCButton m_buttonPaste;
        CMFCButton m_buttonDelete;
        CStatic m_staticNumFiligrees;
        CComboBox m_comboNumFiligrees;
        CButton m_filigreeMenu;
        CInventoryDialog * m_inventoryView;

        CDocument * m_pDocument;
        Character * m_pCharacter;
};
