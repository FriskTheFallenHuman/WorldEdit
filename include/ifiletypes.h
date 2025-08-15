#pragma once

#include <list>
#include <string>
#include "imodule.h"

/**
 * Simple structure to store a file pattern (e.g. "*.map") 
 * along with its name (e.g. "Map files") and extension.
 */
struct FileTypePattern
{
	// The user-friendly name ("Doom 3 Map")
	std::string name;

	// The extension in lowercase ("map")
	std::string extension;

	// The mask pattern ("*.map")
	std::string pattern;

	// Optional icon string, referring to an image in the bitmaps folder
	std::string icon;

	// Constructor with optional initialisation parameters
	FileTypePattern(const std::string& name_ = "", 
					const std::string& extension_ = "", 
					const std::string& pattern_ = "",
					const std::string& icon_ = "") :
		name(name_),
		extension(extension_),
		pattern(pattern_),
		icon(icon_)
	{}
};
typedef std::list<FileTypePattern> FileTypePatterns;

const char* const MODULE_FILETYPES = "FileTypes";

/**
 * Interface for the FileType registry module. This module retains a list of
 * FileTypePattern objects along with their associated module names.
 */
class IFileTypeRegistry :
	public RegisterableModule
{
public:
	/**
	 * greebo: Registers an extension (e.g. "map") for a certain file type ("prefab").
	 * Common file types are "map", "prefab" and "model", each of them can have one 
	 * or more extensions associated in a certain order ("prefab" => "pfb", "map", "reg",
	 * or "map" => "map", "reg", "pfb"). The order is important e.g. for the file 
	 * filters in the Map Open dialog.
	 *
	 * The pattern argument is a structure containing the lowercase extensions as well
	 * as the display name and the filter pattern used in the file chooser dialogs.
	 *
	 * If an extension is already associated with the given filetype, it is ignored.
	 * New extensions are added to the end of the list.
	 *
	 * @param fileType: the file type which an extension should be associated to.
	 * @param pattern: the extension as well as the display name and a pattern ("*.map")
	 */
	virtual void registerPattern(const std::string& fileType, 
								 const FileTypePattern& pattern) = 0;

	/**
	 * Retrieve a list of patterns for the given file type (e.g. "prefab" or "map").
	 *
	 * @returns: a list of FileTypePatterns containing extension, display name, etc.
	 */
	virtual FileTypePatterns getPatternsForType(const std::string& fileType) = 0;

	/**
	 * Tries to find an icon file for the given extension. If not empty,
	 * the returned string refers to a filename in the bitmaps/ folder.
	 */
	virtual std::string getIconForExtension(const std::string& extension) = 0;
};

namespace filetype
{

// Some well-known file type constants
const char* const TYPE_MAP = "map";
const char* const TYPE_MAP_EXPORT = "mapexport";
const char* const TYPE_PREFAB = "prefab";
const char* const TYPE_REGION = "region";
const char* const TYPE_MODEL_EXPORT = "modelexport";
const char* const TYPE_PAK = "pak";

}

inline IFileTypeRegistry& GlobalFiletypes()
{
	static module::InstanceReference<IFileTypeRegistry> _reference(MODULE_FILETYPES);
	return _reference;
}
