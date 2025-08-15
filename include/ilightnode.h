#pragma once

#include <memory>
#include "inode.h"
#include "math/AABB.h"

/**
 * LightNodes derive from this class.
 * It's mainly used to determine the selectable part
 * of the light, which is usually the small "diamond" in
 * the center.
 */
class ILightNode
{
public:
	virtual ~ILightNode() {}

	/// Return the RendererLight instance for this light node
	virtual const RendererLight& getRendererLight() const = 0;

	/// Get the AABB of the Light "Diamond" representation.
	virtual AABB getSelectAABB() const = 0;
};
typedef std::shared_ptr<ILightNode> ILightNodePtr;

inline ILightNodePtr Node_getLightNode(const scene::INodePtr& node)
{
	return std::dynamic_pointer_cast<ILightNode>(node);
}
