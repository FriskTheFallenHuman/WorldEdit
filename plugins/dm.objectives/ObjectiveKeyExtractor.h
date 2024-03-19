#pragma once

#include "Objective.h"
#include "Specifier.h"

#include "scene/Entity.h"

namespace objectives
{

/**
 * Entity Visitor which extracts objective keyvalues (of the form "obj<n>_blah")
 * and populates the given ObjectiveMap with the parsed objective objects.
 */
class ObjectiveKeyExtractor
{
	// Map of number->Objective objects
	ObjectiveMap& _objMap;

public:

	/**
	 * Constructor. Sets the map to populate.
	 */
	ObjectiveKeyExtractor(ObjectiveMap& map)
	: _objMap(map)
	{
		assert(_objMap.empty());
	}

	/**
	 * Entity keyvalue visit function.
	 */
	void operator()(const std::string& key, const std::string& value);

private:
	/**
	 * Sanity-checks and converts the given integer specifier [1..N] number
	 * to the enum range [FIRST_SPECIFIER..MAX_SPECIFIERS).
	 *
	 * @returns: MAX_SPECIFIERS if the given integer is invalid, the valid enum otherwise
	 */
	Specifier::SpecifierNumber getSpecifierNumber(int specNum);
};

}
