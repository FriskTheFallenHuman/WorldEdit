#include "LayerContextMenu.h"

#include "imap.h"
#include "wxutil/menu/IconTextMenuItem.h"

namespace ui
{

	namespace
	{
		const std::string LAYER_ICON("layer.png");
	}

LayerContextMenu::LayerContextMenu(OnSelectionFunc& onSelection) :
	wxMenu(),
	_onSelection(onSelection)
{}

void LayerContextMenu::populate()
{
	_sortedLayers.clear();

	if (!GlobalMapModule().getRoot())
	{
		return;
	}

	// Populate the map with all layer names and IDs
	GlobalMapModule().getRoot()->getLayerManager().foreachLayer([&](int layerId, const std::string& layerName)
	{
		_sortedLayers.insert(SortedLayerMap::value_type(layerName, layerId));
	});

	// Clear all existing items
	for (const MenuItemIdToLayerMapping::value_type& i : _menuItemMapping)
	{
		wxMenu* parentMenu = GetParent();

#ifdef __WXGTK__
		// wxMSW and wxGTK are doing it differently (see comments in createMenuItems() below)
		parentMenu = this;
#endif

		if (parentMenu != nullptr)
		{
			parentMenu->Disconnect(i.first, wxEVT_MENU, wxCommandEventHandler(LayerContextMenu::onActivate), nullptr, this);
		}

		Delete(i.first);
	}

	_menuItemMapping.clear();

	// Create the menu items
	createMenuItems();
}

void LayerContextMenu::createMenuItems()
{
	for (const SortedLayerMap::value_type& i : _sortedLayers)
	{
		// Create a new menuitem
		wxMenuItem* menuItem = new wxutil::IconTextMenuItem(i.first, LAYER_ICON);

		// Add it to the parent menu
		Append(menuItem);

		// remember the layer id for this item
		_menuItemMapping[menuItem->GetId()] = i.second;

		wxMenu* parentMenu = GetParent();

#ifdef __WXGTK__
		// wxMSW and wxGTK are doing it differently (which is always great):
		// in MSW the parent menu (of this class) is firing the events, whereas
		// in GTK+ the submenu (this class) itself is doing that. So let's do an IFDEF
		parentMenu = this;
#endif

		// If we're packed to a parent menu (ourselves acting as submenu), connect the event to the parent
		if (parentMenu != nullptr)
		{
			parentMenu->Connect(menuItem->GetId(), wxEVT_MENU, wxCommandEventHandler(LayerContextMenu::onActivate), NULL, this);
		}
	}
}

void LayerContextMenu::onActivate(wxCommandEvent& ev)
{
	assert(_menuItemMapping.find(ev.GetId()) != _menuItemMapping.end());

	int layerId = _menuItemMapping[ev.GetId()];

	// Pass the call to the function
	_onSelection(layerId);
}

} // namespace ui
