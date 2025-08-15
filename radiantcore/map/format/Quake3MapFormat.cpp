#include "Quake3MapFormat.h"

#include "parser/DefTokeniser.h"

#include "Quake3MapReader.h"
#include "Quake3MapWriter.h"

#include "Doom3MapFormat.h"

#include "module/StaticModule.h"

namespace map
{

// RegisterableModule implementation
const std::string& Quake3MapFormat::getName() const
{
	static std::string _name("Quake3MapLoader");
	return _name;
}

const StringSet& Quake3MapFormatBase::getDependencies() const
{
	static StringSet _dependencies;

	if (_dependencies.empty())
	{
		_dependencies.insert(MODULE_MAPFORMATMANAGER);
	}

	return _dependencies;
}

void Quake3MapFormatBase::initialiseModule(const IApplicationContext& ctx)
{
	// Register ourselves as map format for maps and regions
	GlobalMapFormatManager().registerMapFormat("map", getSharedToThis());
	GlobalMapFormatManager().registerMapFormat("reg", getSharedToThis());
	GlobalMapFormatManager().registerMapFormat("pfb", getSharedToThis());
}

void Quake3MapFormatBase::shutdownModule()
{
	// Unregister now that we're shutting down
	GlobalMapFormatManager().unregisterMapFormat(getSharedToThis());
}

const std::string& Quake3MapFormat::getMapFormatName() const
{
	static std::string _name = "Quake 3";
	return _name;
}

const std::string& Quake3MapFormat::getGameType() const
{
	static std::string _gameType = "quake3";
	return _gameType;
}

IMapReaderPtr Quake3MapFormatBase::getMapReader(IMapImportFilter& filter) const
{
	return std::make_shared<Quake3MapReader>(filter);
}

IMapWriterPtr Quake3MapFormat::getMapWriter() const
{
	return std::make_shared<Quake3MapWriter>();
}


bool Quake3MapFormatBase::allowInfoFileCreation() const
{
	// allow .project/.mapx files to be saved
	return true;
}

bool Quake3MapFormatBase::canLoad(std::istream& stream) const
{
	// Instantiate a tokeniser to read the first few tokens
	parser::BasicDefTokeniser<std::istream> tok(stream);

	try
	{
		// Require the opening brace of the first entity as first token
		tok.assertNextToken("{");

		// That's it for the moment being
		return true;
	}
	catch (parser::ParseException&)
	{}

	return false;
}

module::StaticModuleRegistration<Quake3MapFormat> q3MapModule;

// Quake 3 with alternate brush format

const std::string& Quake3AlternateMapFormat::getMapFormatName() const
{
	static std::string _name = "Quake 3 Alternate";
	return _name;
}

const std::string& Quake3AlternateMapFormat::getGameType() const
{
	static std::string _gameType = "quake3alternate";
	return _gameType;
}

const std::string& Quake3AlternateMapFormat::getName() const
{
	static std::string _name("Quake3AlternateMapLoader");
	return _name;
}

IMapWriterPtr Quake3AlternateMapFormat::getMapWriter() const
{
	return std::make_shared<Quake3AlternateMapWriter>();
}


module::StaticModuleRegistration<Quake3AlternateMapFormat> q3AlternateMapModule;

} // namespace map
