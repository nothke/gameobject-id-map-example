#include <iostream>
#include <assert.h>
#include <vector>
#include <map>

#define SWAP_BACK

using u32 = unsigned int;

template<typename T>
struct PersistentReferenceManager;

template <typename T>
struct PersistentReference {
	PersistentReferenceManager<T>& manager;
	u32 id{ 0 };

	T& Get() {
		return manager.GetPersistentReference(id);
	}
};

template<typename T>
struct PersistentReferenceManager {
	std::vector<T> models; // this is our actual contiguous array of data
	std::map<u32, u32> map; // a dictionary that maps ids to models vector index

	u32 last{ 0 }; // incremented to always provide a unique id for new objects

	static constexpr u32 NULL_ID{ 0 }; // a 0 should map to invalid (the gameobject is removed)

	// to preallocate or not to preallocate?
	//PersistentReferenceManager() {
	//	models.reserve(12);
	//}

	// Used only by the GameObject
	T& __GetModelById(u32 id) {
		assert(map.contains(id)); // Attempting to reference dead object
		assert(map.at(id) != NULL_ID); // Attempting to reference dead object

		return models[static_cast<size_t>(map.at(id) - 1)];
	}

	T& GetPersistentReference(u32 id) {
		return __GetModelById(id);
	}

	// Creates a new GameObject in Scene
	PersistentReference<T> Create() {
		models.emplace_back();

		// the new object will always be last
		u32 newIndex = static_cast<u32>(models.size());
		map[last] = newIndex;

		return PersistentReference<T>{ *this, last++ };
	}

	// Removes a GameObject from this Scene
	void Destroy(const PersistentReference<T>& go) {
		u32 mi = map.at(go.id) - 1; // model index

#ifndef SWAP_BACK
		models.erase(models.begin() + mi);

		map.erase(go.id);

		// loop through all map and remap every higher index
		for (auto& pair : map)
		{
			if (pair.second > mi)
				pair.second--;
		}
#else
		// find which object id is the last model
		// this is needed because we don't store id in the Model
		u32 lastId = 0;
		for (const auto& [id, i] : map)
		{
			if (i == models.size()) // no -1 because i is offset
			{
				lastId = id;
				break;
			}
		}

		// swap deleted object with last
		std::swap(models[mi], models.back());
		models.pop_back();

		map.erase(go.id);
		map[lastId] = mi + 1; // swap id of old last object
#endif
	}

	PersistentReference<T> GetById(u32 id)
	{
		assert(map.contains(id)); // Attempting to reference dead object
		assert(map.at(id) != NULL_ID); // Attempting to reference dead object

		return { *this, id };
	}
};

// This is our actual object that holds some data
struct Model {
	int a{ 0 }, b{ 0 };
	float x{ 0 }, y{ 0 }, z{ 0 };
};

struct Texture {
	u32 handle;
};

using GameObject = PersistentReference<Model>;
using Scene = PersistentReferenceManager<Model>;

using TextureAsset = PersistentReference<Texture>;
using TextureManager = PersistentReferenceManager<Texture>;

void DebugScene(Scene& scene)
{
	std::cout << "----- DEBUG -----" << "\n";
	std::cout << "\tIDs" << "\n";

	for (size_t i = 0; i < scene.models.size(); i++)
	{
		std::cout << "\t\tModel: index: " << i << ", a = " << scene.models[i].a << ", b = " << scene.models[i].b << "\n";
	}

	std::cout << "\tMAP" << "\n";

	for (auto& pair : scene.map)
	{
		std::cout << "\t\tid: " << pair.first << ", mi= " << pair.second << "\n";
	}
}

int main(int argc, char* argv[]) {
	Scene scene;

	// Create 3 objects

	auto go = scene.Create();

	go.Get().a = 10;
	go.Get().b = 20;

	auto go2 = scene.Create();
	go2.Get().a = 5;

	auto go3 = scene.Create();
	go3.Get().a = 6;

	std::cout << "Created 3 objects\n";
	DebugScene(scene);

	// Destroy one object
	scene.Destroy(go2);

	go3.Get().a = 666;

	std::cout << "Destroyed object 2\n";
	DebugScene(scene);

	// Creating some more objects
	for (size_t i = 0; i < 5; i++)
	{
		auto _go = scene.Create();
		_go.Get().a = 100 + i;
	}

	std::cout << "Created 5 more objects\n";
	DebugScene(scene);

	// Removing objects
	for (size_t i = 3; i < 6; i++)
	{
		GameObject _go = scene.GetById(i);
		scene.Destroy(_go);
	}

	DebugScene(scene);

	// Creating many more objects
	for (size_t i = 0; i < 200; i++)
	{
		auto _go = scene.Create();
		_go.Get().a = 200 + i;
	}

	// Removing many objects
	for (size_t i = 50; i < 150; i++)
	{
		auto _go = scene.GetById(i);
		scene.Destroy(_go);
	}

	go3.Get().b = 12345;

	DebugScene(scene);

	auto go4 = scene.Create();
	go4.Get().a = 32;
	auto go4copy = go4;
	go4copy.Get().b = 32;

	DebugScene(scene);

	scene.Destroy(go4);
	//go4.Get().b = 12; // Uncommenting should error

	{
		TextureManager textures;

		auto tex = textures.Create();
	}

	return EXIT_SUCCESS;
}