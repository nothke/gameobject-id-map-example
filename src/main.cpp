#include <iostream>
#include <assert.h>
#include <vector>
#include <map>

struct Model {
	bool alive{ false };
	int a{ 0 };
	int b{ 0 };
};

struct Scene;

using u32 = unsigned int;

// Trivially copiable
struct GameObject {
	Scene& scene;
	u32 id{ 0 }; // maps into the map

	Model& Get();
};


struct Scene {
	std::vector<Model> models;
	std::map<u32, u32> map; // id to models vector index

	u32 last{ 0 };

	static constexpr u32 NULL_ID{ 0 }; // a 0 should map to invalid (the gameobject is removed)

	Scene() {
		models.reserve(12);
	}

	Model& __GetModelById(u32 id) {
		assert(map.at(id) != NULL_ID);
		return models[map.at(id) - 1];
	}

	GameObject Create() {
		auto& model = models.emplace_back();
		model.alive = true;

		// the new object will always be last
		u32 newIndex = static_cast<u32>(models.size());
		map[last] = newIndex;

		return GameObject{ *this, last++ };
	}

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

	auto go = scene.Create();

	go.Get().a = 10;
	go.Get().b = 20;

	auto go2 = scene.Create();
	go2.Get().a = 5;

	auto go3 = scene.Create();
	go3.Get().a = 6;

	DebugScene(scene);

	scene.Destroy(go2);

	DebugScene(scene);

	for (size_t i = 0; i < 5; i++)
	{
		auto _go = scene.Create();
		_go.Get().a = 100 + i;
	}

	DebugScene(scene);

	for (size_t i = 3; i < 6; i++)
	{
		GameObject _go = scene.GetById(i);
		scene.Destroy(_go);
	}

	DebugScene(scene);

	for (size_t i = 0; i < 20; i++)
	{
		auto _go = scene.Create();
		_go.Get().a = 200 + i;
	}

	DebugScene(scene);

	return EXIT_SUCCESS;
}