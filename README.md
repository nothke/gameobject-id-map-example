A simple example of a game object system for a game engine written in C++.

The purpose of this example is to show how to have a persistent reference to a game object that can be easily used by a developer, no matter how many objects are created or destroyed while keeping the actual data contiguous (in a single array) and compact (without holes). This is done through a light "reference id" and a scene which manages a map that directs those ids to the correct data.

There are 3 classes:
- _Models_ contain the data, and are stored in a compact contiguous vector (in _Scene_), that is easy for the engine to loop through and draw objects
- _GameObjects_ are persistent references to _Models_ that are also trivially copyable and meant to be used by the developer
- _Scene_ contains the _Models_ and a map that maps _GameObjects_ to _Models_

Now, the user can:
```c++
// create 2 objects
GameObject go1 = scene.Create(); 
GameObject go2 = scene.Create();

// Access and change data with .Get().
go1.Get().a = 10; 

scene.Destroy(go1);

// Even though data of go2 has moved in memory, it can still be safely accessed
go2.get().a = 20;

go1.Get().a = 23 // OOOPS, error: accessing dead element!
```

This example also uses a swap-back O(1) removal when destroying objects.

There is a slight overhead when getting model data (compared to a raw pointer), but it is one of the most commonly used methods of working with game objects. You can also safely get a pointer to the Model directly and modify the data AS LONG as there are no structural changes between getting the pointer and using it.

### Templateized

`templateized` branch features a templateized version of this system, so it can be reused for other assets.

GameObject and Scene are replaced by `PersistentReference<Model>` and `PersistentReferenceManager<Model>`, respectively. GameObject and Scene now just alias those, so user code doesn't change at all.