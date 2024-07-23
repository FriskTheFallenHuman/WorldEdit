#include "PrefPage.h"

#include "i18n.h"
#include "ipreferencesystem.h"

#include <wx/sizer.h>
#include <wx/stattext.h>

#include "PreferenceItem.h"

namespace ui
{

PrefPage::PrefPage(wxWindow* parent, const IPreferencePage& settingsPage) :
	wxPanel(parent, wxID_ANY),
	_settingsPage(settingsPage)
{
	// Create the overall panel
	SetSizer(new wxBoxSizer(wxVERTICAL));

	// 12 pixel border
	wxBoxSizer* overallVBox = new wxBoxSizer(wxVERTICAL);
	GetSizer()->Add(overallVBox, 1, wxEXPAND | wxALL, 12);

	// Create the label, unless the page is empty
	if (!settingsPage.isEmpty())
	{
		wxStaticText* titleLabel = new wxStaticText(this, wxID_ANY, _settingsPage.getTitle());
		titleLabel->SetFont(titleLabel->GetFont().Bold());
		overallVBox->Add(titleLabel, 0, wxBOTTOM, 12);
	}

	_table = new wxFlexGridSizer(1, 2, 6, 12);
	overallVBox->Add(_table, 1, wxEXPAND | wxLEFT, 6); // another 12 pixels to the left

	settingsPage.foreachItem([&](const IPreferenceItemBase::Ptr& item)
	{
		createItemWidgets(item);
	});
}

void PrefPage::saveChanges()
{
	_registryBuffer.commitChanges();
}

void PrefPage::resetValues()
{
	_registryBuffer.clear();

	_resetValuesSignal();
}

void PrefPage::appendNamedWidget(const std::string& name, wxWindow* widget, bool useFullWidth)
{
	if (_table->GetItemCount() > 0)
	{
		// Add another row
		_table->SetRows(_table->GetRows() + 1);
	}

	_table->Add(new wxStaticText(this, wxID_ANY, name), 0, wxALIGN_CENTRE_VERTICAL);
	_table->Add(widget, useFullWidth ? 1 : 0, wxEXPAND);
}

void PrefPage::createItemWidgets(const IPreferenceItemBase::Ptr& item)
{
	// Construct a generic item and pass the common values
	PreferenceItem widget(this, item->getRegistryKey(), _registryBuffer, _resetValuesSignal);

	// Switch on the item type
	if (std::dynamic_pointer_cast<IPreferenceLabel>(item))
	{
		wxWindow* label = widget.createLabel(item->getLabel());

		appendNamedWidget("", label);
	}
	else if (std::dynamic_pointer_cast<IPreferenceEntry>(item))
	{
		appendNamedWidget(item->getLabel(), widget.createEntry());
	}
	else if (std::dynamic_pointer_cast<IPreferenceCheckbox>(item))
	{
		wxWindow* checkbox = widget.createCheckbox(item->getLabel());

		appendNamedWidget("", checkbox);
	}
	else if (std::dynamic_pointer_cast<IPreferenceCombobox>(item))
	{
		auto info = std::dynamic_pointer_cast<IPreferenceCombobox>(item);

		wxWindow* combobox = widget.createCombobox(info->getValues(), info->storeValueNotIndex());

		appendNamedWidget(item->getLabel(), combobox, false);
	}
	else if (std::dynamic_pointer_cast<IPreferencePathEntry>(item))
	{
		auto info = std::dynamic_pointer_cast<IPreferencePathEntry>(item);

		wxWindow* pathEntry = widget.createPathEntry(info->browseDirectories());

		appendNamedWidget(item->getLabel(), pathEntry);
	}
	else if (std::dynamic_pointer_cast<IPreferenceSpinner>(item))
	{
		auto info = std::dynamic_pointer_cast<IPreferenceSpinner>(item);

		wxWindow* spinner = widget.createSpinner(info->getLower(), info->getUpper(), info->getFraction());

		appendNamedWidget(item->getLabel(), spinner);
	}
	else if (std::dynamic_pointer_cast<IPreferenceSlider>(item))
	{
		auto info = std::dynamic_pointer_cast<IPreferenceSlider>(item);

		wxWindow* slider = widget.createSlider(info->getLower(), info->getUpper(),
			info->getStepIncrement(), info->getPageIncrement());

		appendNamedWidget(item->getLabel(), slider);
	}
}

} // namespace ui
