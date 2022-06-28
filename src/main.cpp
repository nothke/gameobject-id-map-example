#include <iostream>
#include <assert.h>
#include <vector>
#include <map>

using u32 = unsigned int;

// This is our actual object that holds some data
struct Model {
	int a{ 0 }, b{ 0 };
	float x{ 0 }, y{ 0 }, z{ 0 };
};

struct Scene; // just a forward declaration necessary for GameObject

// A trivially copiable wrapper to the Model
struct GameObject {
	Scene& scene;	// reference to the scene
	u32 id{ 0 };	// maps into the map

	Model& Get(); // just forward declared here necessary for Scene (implemented below Scene)
};

// The object that holds our model and map
struct Scene {
	std::vector<Model> models; // this is our actual contiguous array of data
	std::map<u32, u32> map; // a dictionary that maps ids to models vector index

	u32 last{ 0 }; // incremented to always provide a unique id for new objects

	static constexpr u32 NULL_ID{ 0 }; // a 0 should map to invalid (the gameobject is removed)

	Scene() {
		models.reserve(12);
	}

	// Used only by the GameObject
	Model& __GetModelById(u32 id) {
		assert(map.at(id) != NULL_ID);
		return models[map.at(id) - 1];
	}

	// Creates a new GameObject in Scene
	GameObject Create() {
		auto& model = models.emplace_back();

		// the new object will always be last
		u32 newIndex = static_cast<u32>(models.size());
		map[last] = newIndex;

		return GameObject{ *this, last++ };
	}

	// Removes a GameObject from this Scene
	void Destroy(const GameObject& go) {
		u32 mi = map.at(go.id) - 1; // model index
		models.erase(models.begin() + mi);

		map.erase(go.id);

		// loop through all map and remap every higher index
		for (auto& pair : map)
		{
			if (pair.second > mi)
				pair.second--;
		}
	}

	GameObject GetById(u32 id)
	{
		assert(map.at(id) != NULL_ID);
		return { *this, id };
	}
};

Model& GameObject::Get()
{
	return scene.__GetModelById(id);
}

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

	DebugScene(scene);

	// Destroy one object
	scene.Destroy(go2);

	DebugScene(scene);

	// Creating some more objects
	for (size_t i = 0; i < 5; i++)
	{
		auto _go = scene.Create();
		_go.Get().a = 100 + i;
	}

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

	DebugScene(scene);

	return EXIT_SUCCESS;
}