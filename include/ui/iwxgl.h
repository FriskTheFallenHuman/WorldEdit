#pragma once

#include "imodule.h"

namespace wxutil { class GLWidget; }

namespace ui
{

// Application wide manager for wxGLWidgets
// All GLWidgets need to register themselves to this module
// since the shared wxGLContext creation will be bound to 
// the first registered widget
class IWxGLWidgetManager :
	public RegisterableModule
{
public:
	/// Registers a GL widget, storing the shared context if necessary
	virtual void registerGLWidget(wxutil::GLWidget* widget) = 0;

	/// Notifies the GL module that a GLWidget has been destroyed
	virtual void unregisterGLWidget(wxutil::GLWidget* widget) = 0;
};

}

const char* const MODULE_WXGLWIDGET_MANAGER("wxGLWidgetManager");

inline ui::IWxGLWidgetManager& GlobalWxGlWidgetManager()
{
	static module::InstanceReference<ui::IWxGLWidgetManager> _reference(MODULE_WXGLWIDGET_MANAGER);
	return _reference;
}
