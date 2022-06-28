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

// Trivially copiable
struct GameObject {
	Scene& scene;
	int id;

	Model& Get();
};

struct Scene {
	std::vector<Model> models;
	std::map<int, Model*> map;

	Scene() {
		models.reserve(64);
	}

	Model& GetById(int id) {
		assert(map[id] != nullptr);
		return *map[id];
	}

	GameObject Create() {
		auto& model = models.emplace_back();
		model.alive = true;

		int index = models.size() - 1;
		std::cout << index << "\n";
		map[index] = &model;
		return GameObject{ *this, index };
	}

	void Destroy(GameObject& go) {
		int index = go.id;
		models.erase(models.begin() + index);

		map[index] = nullptr; // remap this one to nullptr

		// remap all the following ones to one less
		for (size_t i = index + 1; i < map.size(); i++)
		{
			map[i] = &models[i - 1];
		}
	}
};

Model& GameObject::Get()
{
	return scene.GetById(id);
}

void DebugScene(Scene& scene)
{
	std::cout << "IDs" << "\n";

	for (size_t i = 0; i < scene.models.size(); i++)
	{
		std::cout << "Model: index: " << i << ", a= " << scene.models[i].a << ", b= " << scene.models[i].b << "\n";
	}

	std::cout << "MAP" << "\n";

	for (auto& pair : scene.map)
	{
		std::cout << "id: " << pair.first << ", ptr= " << pair.second << "\n";
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

	go3.Get().a = 100;

	DebugScene(scene);

	return EXIT_SUCCESS;
}