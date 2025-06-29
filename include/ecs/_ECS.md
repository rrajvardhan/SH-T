# 🧱 Using the Overseer ECS Framework (2D Edition)

## ⚙️ Setup

### 1. Initialize the ECS

```cpp
Overseer overseer;
overseer.init();
```

This sets up:

* `ComponentManager`
* `EntityManager`
* `SystemManager`

---

### 2. Register Your Components

You **must register** each component type before use.

```cpp
overseer.registerComponent<Gravity>();
overseer.registerComponent<RigidBody>();
overseer.registerComponent<Transform>();
```

---

### 3. Define and Register Systems

Define a system that extends `System`:

```cpp
class PhysicsSystem : public System {
public:
    void update(float dt, Overseer& overseer) {
        for (auto entity : _entities) {
            auto& rb = overseer.getComponent<RigidBody>(entity);
            auto& tr = overseer.getComponent<Transform>(entity);
            const auto& g = overseer.getComponent<Gravity>(entity);

            rb.velocity += g.force * dt;
            tr.position += rb.velocity * dt;
        }
    }
};
```

Register it:

```cpp
auto physicsSystem = overseer.registerSystem<PhysicsSystem>();
```

Set the component **signature** (bitmask) it needs:

```cpp
Signature sig;
sig.set(overseer.getComponentType<Gravity>());
sig.set(overseer.getComponentType<RigidBody>());
sig.set(overseer.getComponentType<Transform>());
overseer.setSystemSignature<PhysicsSystem>(sig);
```

---

### 4. Create Entities and Add Components

```cpp
Entity e = overseer.createEntity();

overseer.addComponent(e, Gravity{Vector2D(0.0f, -9.81f)});
overseer.addComponent(e, RigidBody{Vector2D(), Vector2D()});
overseer.addComponent(e, Transform{Vector2D(100.0f, 100.0f), Vector2D(1.0f, 1.0f)});
```

You can also spawn hundreds/thousands of entities with randomized data.

---

### 5. Run the Simulation

In your game loop:

```cpp
while (!quit) {
    float dt = calculateDeltaTime(); // Your timing mechanism
    physicsSystem->update(dt, overseer);
}
```

---

## 🧠 Notes

* `Signature` is a bitset that defines which entities a system should process.
* Components are stored in tightly-packed arrays for cache performance.
* Systems automatically track relevant entities based on signatures.

---

## 📦 Example Components (2D)

```cpp
struct Vector2D {
    float x;
    float y;

    Vector2D() : x(0), y(0) {}
    Vector2D(float x, float y) : x(x), y(y) {}

    Vector2D operator+(const Vector2D& rhs) const { return Vector2D(x + rhs.x, y + rhs.y); }
    Vector2D operator*(float scalar) const { return Vector2D(x * scalar, y * scalar); }
    Vector2D& operator+=(const Vector2D& rhs) { x += rhs.x; y += rhs.y; return *this; }
};

struct Gravity {
    Vector2D force;
};

struct RigidBody {
    Vector2D velocity;
    Vector2D acceleration;
};

struct Transform {
    Vector2D position;
    Vector2D scale;
};
```

---
