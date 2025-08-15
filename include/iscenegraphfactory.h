#pragma once

#include "iscenegraph.h"

namespace scene
{

/**
 * greebo: The scenegraph factory can be used to generate
 * new instances of WorldEdit's main scene manager.
 *
 * A Scenegraph consists of [0..N] scene::INodes, forming
 * an acyclic graph. There is one main scenegraph in WorldEdit
 * accessible through GlobalSceneGraph(), but it's possible to have
 * more than this one, used for preview scenes for example.
 */
class ISceneGraphFactory :
	public RegisterableModule
{
public:
	/**
	 * Instantiates a new scenegraph.
	 */
	virtual GraphPtr createSceneGraph() = 0;
};

} // namespace

const char* const MODULE_SCENEGRAPHFACTORY = "SceneGraphFactory";

// Global accessor to the rendersystem factory module
inline scene::ISceneGraphFactory& GlobalSceneGraphFactory()
{
	static module::InstanceReference<scene::ISceneGraphFactory> _reference(MODULE_SCENEGRAPHFACTORY);
	return _reference;
}
