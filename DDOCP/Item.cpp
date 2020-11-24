// Item.cpp
//
#include "StdAfx.h"
#include "Item.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"
#include <algorithm>
#
#define DL_ELEMENT Item

namespace
{
    const wchar_t f_saxElementName[] = L"Item";
    DL_DEFINE_NAMES(Item_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Item::Item() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Item_PROPERTIES)
}

Item::Item(const XmlLib::SaxString & objectName) :
    XmlLib::SaxContentElement(objectName, f_verCurrent)
{
    DL_INIT(Item_PROPERTIES)
}

DL_DEFINE_ACCESS(Item_PROPERTIES)

XmlLib::SaxContentElementInterface * Item::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Item_PROPERTIES)

    return subHandler;
}

void Item::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Item_PROPERTIES)
    // fix: older files can contain augments that have erroneous "Value"
    // fields in them. Clear these on load if present
    std::vector<ItemAugment>::iterator it = m_Augments.begin();
    while (it != m_Augments.end())
    {
        if ((*it).HasSelectedAugment())
        {
            const Augment & augment = FindAugmentByName((*it).SelectedAugment());
            if (!augment.HasEnterValue())
            {
                // this augment should not have a Value field
                (*it).Clear_Value();
            }
        }
        ++it;
    }
}

void Item::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Item_PROPERTIES)
    writer->EndElement();
}

bool Item::CanEquipToSlot(InventorySlotType slot) const
{
    bool canEquipToSlot = false;
    switch (slot)
    {
    case Inventory_Arrows:
        canEquipToSlot = m_Slots.HasArrow();
        break;
    case Inventory_Armor:
        canEquipToSlot = m_Slots.HasArmor();
        break;
    case Inventory_Belt:
        canEquipToSlot = m_Slots.HasBelt();
        break;
    case Inventory_Boots:
        canEquipToSlot = m_Slots.HasBoots();
        break;
    case Inventory_Bracers:
        canEquipToSlot = m_Slots.HasBracers();
        break;
    case Inventory_Cloak:
        canEquipToSlot = m_Slots.HasCloak();
        break;
    case Inventory_Gloves:
        canEquipToSlot = m_Slots.HasGloves();
        break;
    case Inventory_Goggles:
        canEquipToSlot = m_Slots.HasGoggles();
        break;
    case Inventory_Helmet:
        canEquipToSlot = m_Slots.HasHelmet();
        break;
    case Inventory_Necklace:
        canEquipToSlot = m_Slots.HasNecklace();
        break;
    case Inventory_Quiver:
        canEquipToSlot = m_Slots.HasQuiver();
        break;
    case Inventory_Ring1:
    case Inventory_Ring2:
        canEquipToSlot = m_Slots.HasRing();
        break;
    case Inventory_Trinket:
        canEquipToSlot = m_Slots.HasTrinket();
        break;
    case Inventory_Weapon1:
        canEquipToSlot = m_Slots.HasWeapon1();
        break;
    case Inventory_Weapon2:
        canEquipToSlot = m_Slots.HasWeapon2();
        break;
    }
    return canEquipToSlot;
}

void Item::AddImage(CImageList * pIL) const
{
    CImage image;
    HRESULT result = LoadImageFile(
            IT_item,
            m_Icon,
            &image);
    if (result == S_OK)
    {
        if (image.GetHeight() != 32
                || image.GetWidth() != 32)
        {
            ::OutputDebugString("Image file incorrect size - ");
            ::OutputDebugString(m_Icon.c_str());
            ::OutputDebugString("\n");
        }
        CBitmap bitmap;
        bitmap.Attach(image.Detach());
        int ret = pIL->Add(&bitmap, c_transparentColour);  // standard mask color (purple)
        ASSERT(ret >= 0);
    }
    else
    {
        ::OutputDebugString("Image file failed to load - ");
        ::OutputDebugString(m_Icon.c_str());
        ::OutputDebugString("\n");
    }
}

void Item::VerifyObject() const
{
    bool ok = true;
    std::stringstream ss;
    ss << "=====" << m_Name << "=====\n";
    if (!ImageFileExists(IT_item, Icon()))
    {
        ss << "Item is missing image file \"" << Icon() << "\"\n";
        ok = false;
    }
    // check item specific effects
    if (m_hasItemEffects)
    {
        ok &= m_ItemEffects.VerifyObject(&ss);
    }
    // check the item effects also
    std::vector<Effect>::const_iterator it = m_Effects.begin();
    while (it != m_Effects.end())
    {
        ok &= (*it).VerifyObject(&ss);
        ++it;
    }
    // verify its DC objects
    std::list<DC>::const_iterator edcit = m_EffectDC.begin();
    while (edcit != m_EffectDC.end())
    {
        ok &= (*edcit).VerifyObject(&ss);
        ++edcit;
    }
    // verify any augments
    std::vector<ItemAugment>::const_iterator iacit = m_Augments.begin();
    while (iacit != m_Augments.end())
    {
        ok &= (*iacit).VerifyObject(&ss);
        ++iacit;
    }

    if (!ok)
    {
        ::OutputDebugString(ss.str().c_str());
    }
}

void Item::CopyUserSetValues(const Item & original)
{
    // when updating an item to the latest version there are certain user values
    // which we need to copy
    // sentient Jewel and augments/upgrade slots
    m_hasSentientIntelligence = original.m_hasSentientIntelligence;
    m_SentientIntelligence = original.m_SentientIntelligence;
    m_Augments = original.m_Augments;
    m_SlotUpgrades = original.m_SlotUpgrades;
}

bool Item::ContainsSearchText(const std::string & searchText) const
{
    bool bHasSearchText = false;
    // we have to search all of the following text fields of this item:
    // Name
    // Description
    // Drop Location
    // EffectDescription(s)
    bHasSearchText |= SearchForText(Name(), searchText);
    bHasSearchText |= SearchForText(Description(), searchText);
    if (HasMinorArtifact())
    {
        bHasSearchText |= SearchForText("Minor Artifact", searchText);
    }
    if (m_hasDropLocation)
    {
        bHasSearchText |= SearchForText(DropLocation(), searchText);
    }
    std::list<std::string>::const_iterator it = m_EffectDescription.begin();
    while (!bHasSearchText && it != m_EffectDescription.end())
    {
        bHasSearchText |= SearchForText((*it), searchText);
        ++it;
    }
    return bHasSearchText;
}

bool Item::SearchForText(std::string source, const std::string & find) const
{
    // the search is done in all lower case
    std::transform(source.begin(), source.end(), source.begin(), ::tolower);
    bool bTextPresent = (source.find(find.c_str()) != std::string::npos);
    return bTextPresent;
}

