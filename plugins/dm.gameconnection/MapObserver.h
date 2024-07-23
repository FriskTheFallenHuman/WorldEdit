#pragma once

#include "icommandsystem.h"
#include "iscenegraph.h"
#include "scene/Entity.h"
#include "DiffStatus.h"

namespace gameconn
{

/**
 * Private for GameConnection class: do not use directly!
 * Observer for "update map" / "hot reload" feature.
 * It follows all changes done to the map, remembering entities to be included into diff on next map update.
 */
class MapObserver
{
public:
    ~MapObserver();

    //enable/disable the observer
    void setEnabled(bool enabled);

    //returns whether scene observer is enabled now
    bool isEnabled() const;

    //consider all pending changed "applied" right now
    //(clears list of pending changes)
    void clear();

    //returns pending entity change since last clear (or since enabled)
    const DiffEntityStatuses& getChanges() const;

private:
    //receives events about entity changes
    void entityUpdated(const std::string& name, const DiffStatus& diff);
    //add/remove entity observers on the set of entity nodes
    void enableEntityObservers(const std::vector<EntityNodePtr>& entityNodes);
    void disableEntityObservers(const std::vector<EntityNodePtr>& entityNodes);

    //the observer put onto global scene
    std::unique_ptr<scene::Graph::Observer> _sceneObserver;
    //observers put on every entity on scene
    std::map<EntityNode*, Entity::Observer*> _entityObservers;		//note: values owned
    //set of entities with changes since last clear
    DiffEntityStatuses _entityChanges;

    //internal classes can call private methods
    friend class MapObserver_EntityObserver;
    friend class MapObserver_SceneObserver;
};

}
