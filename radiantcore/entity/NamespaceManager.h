#pragma once

#include "scene/Entity.h"
#include "inamespace.h"

#include <map>
#include "scene/Entity.h"
#include "KeyValueObserver.h"
#include "NameKeyObserver.h"
#include "util/Noncopyable.h"

namespace entity {

class NamespaceManager :
	public Entity::Observer,
	public Namespaced,
	public util::Noncopyable
{
private:
	INamespace* _namespace;

	// The attached entity
	Entity& _entity;

	// All the observed key values of the entity get remembered
	// This prevents having to traverse all the keyvalues again when changing namespaces
	typedef std::map<std::string, EntityKeyValue*> KeyValues;
	KeyValues _nameKeys;

	typedef std::map<EntityKeyValue*, NameKeyObserverPtr> NameKeyObserverMap;
	NameKeyObserverMap _nameKeyObservers;

	typedef std::map<EntityKeyValue*, KeyValueObserverPtr> KeyValueObserverMap;
	KeyValueObserverMap _keyValueObservers;

	// lock for this class to avoid double-updates
	bool _updateMutex;

	// The key defining a name spawnarg, usually "name"
	std::string _nameKey;

public:
	NamespaceManager(Entity& entity);

	~NamespaceManager();

	// Gets/sets the namespace of this named object
	void setNamespace(INamespace* space);
	INamespace* getNamespace() const;

	void attachNames();
	void detachNames();

	void connectNameObservers();
	void disconnectNameObservers();

	// Returns the name of this entity
	std::string getName() const;

	// Changes the name of this entity
	void changeName(const std::string& newName);

	/**
	 * greebo: This gets called as soon as a new entity key/value gets added
	 * to the attached entity.
	 *
	 * The routine saves all relevant keyvalues and attaches the
	 * "name keys" to the Namespace.
	 *
	 * Note: Entity::Observer implementation
	 */
	void onKeyInsert(const std::string& key, EntityKeyValue& value);

	/**
	 * greebo: Gets called by the observed Entity when a value is erased from
	 * the list of spawnargs.
	 *
	 * Note: Entity::Observer implementation
	 */
	void onKeyErase(const std::string& key, EntityKeyValue& value);

private:
	/**
	 * greebo: returns TRUE if the given key is recognised as "name" for the
	 * selected game type.
	 */
	bool keyIsName(const std::string& key);

	// Some keyvalues are not referring to names, but to entityDefs, those should not
	// change themselves if an incidentially matching name is changed
	bool keyIsReferringToEntityDef(const std::string& key);

	void detachNameKeys();

	void attachKeyObservers();
	void detachKeyObservers();

	void attachKeyToNamespace(const std::string& key, EntityKeyValue& keyValue);
	void detachKeyFromNamespace(const std::string& key, EntityKeyValue& keyValue);

	void attachKeyObserver(const std::string& key, EntityKeyValue& keyValue);
	void detachKeyObserver(const std::string& key, EntityKeyValue& keyValue);
};

} // namespace entity
