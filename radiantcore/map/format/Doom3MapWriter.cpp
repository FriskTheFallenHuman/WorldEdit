#include "Doom3MapWriter.h"

#include "igame.h"
#include "scene/EntityNode.h"

#include "primitivewriters/BrushDef3Exporter.h"
#include "primitivewriters/PatchDefExporter.h"

#include "Doom3MapFormat.h"

namespace map
{

namespace
{

// Escape the line break characters in the given input string to \n
inline std::string escapeLineBreaks(const std::string& input)
{
	return string::replace_all_copy(input, "\n", "\\n");
}

}

Doom3MapWriter::Doom3MapWriter() :
	_entityCount(0),
	_primitiveCount(0)
{}

void Doom3MapWriter::beginWriteMap(const scene::IMapRootNodePtr& root, std::ostream& stream)
{
	// Write the version tag
    stream << "Version " << MAP_VERSION_D3 << std::endl;
}

void Doom3MapWriter::endWriteMap(const scene::IMapRootNodePtr& root, std::ostream& stream)
{
	// nothing
}

void Doom3MapWriter::beginWriteEntity(const EntityNodePtr& entity, std::ostream& stream)
{
	// Write out the entity number comment
	stream << "// entity " << _entityCount++ << std::endl;

	// Entity opening brace
	stream << "{" << std::endl;

	// Entity key values
	writeEntityKeyValues(entity, stream);
}

void Doom3MapWriter::writeEntityKeyValues(const EntityNodePtr& entity, std::ostream& stream)
{
	// Export the entity key values
    entity->getEntity().forEachKeyValue([&](const std::string& key, const std::string& value)
    {
        stream << "\"" << key << "\" \"" << escapeLineBreaks(value) << "\"" << std::endl;
    });
}

void Doom3MapWriter::endWriteEntity(const EntityNodePtr& entity, std::ostream& stream)
{
	// Write the closing brace for the entity
	stream << "}" << std::endl;

	// Reset the primitive count again
	_primitiveCount = 0;
}

void Doom3MapWriter::beginWriteBrush(const IBrushNodePtr& brush, std::ostream& stream)
{
	// Primitive count comment
	stream << "// primitive " << _primitiveCount++ << std::endl;

	// Export brushDef3 definition to stream
	BrushDef3Exporter::exportBrush(stream, brush);
}

void Doom3MapWriter::endWriteBrush(const IBrushNodePtr& brush, std::ostream& stream)
{
	// nothing
}

void Doom3MapWriter::beginWritePatch(const IPatchNodePtr& patch, std::ostream& stream)
{
	// Primitive count comment
	stream << "// primitive " << _primitiveCount++ << std::endl;

	// Export patch here _mapStream
	PatchDefExporter::exportPatch(stream, patch);
}

void Doom3MapWriter::endWritePatch(const IPatchNodePtr& patch, std::ostream& stream)
{
	// nothing
}

} // namespace
