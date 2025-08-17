#pragma once

#include "ifiletypes.h"
#include <map>

/**
 * Implementation of the file type registry, associating file types
 * with FileTypePatterns. 
 *
 * <type> => [ <pattern>, <pattern>, ... ]
 *
 * "map" => [	("Map", "*.map", "map", ""), 
 *			    ("Map Backup", "*.bak", "bak", "") ]
 * "prefab" => [ ("Prefab", "*.pfb", "pfb", "") ]
 * "region" => [ ("Region", "*.reg", "reg", "") ]
 *
 * It is used by the wxutil::FileChooser to populate the file type dropdown list.
 */
class FileTypeRegistry :
	public IFileTypeRegistry
{
private:
	typedef std::map<std::string, FileTypePatterns> FileTypes;
	FileTypes _fileTypes;

public:
	// IFileTypeRegistry implementation
	void registerPattern(const std::string& fileType, const FileTypePattern& pattern) override;

	FileTypePatterns getPatternsForType(const std::string& fileType) override;

	std::string getIconForExtension(const std::string& extension) override;
	
	// RegisterableModule implementation
	const std::string& getName() const override;
	const StringSet& getDependencies() const override;
	void initialiseModule(const IApplicationContext& ctx) override;
};
