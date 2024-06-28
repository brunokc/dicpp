# DI/C++
_An implementation of automatic dependency injection in C++_

The DI/C++ project demostrates how to automatically inject dependencies onto
components that are added to a Service Host. A simple example should make it
simpler to understand:

Starting with an empty Service Host, supposed that a few self-contained services 
are added:

```c++
struct IFoo { ... };
class Foo : public IFoo { ... };

struct IBar { ... };
class Bar : public IBar { ... };

ServiceHost host;
host.RegisterService<IFoo, Foo>();
host.RegisterService<IBar, Bar>();
```

Then a new service `ServiceA` is created, that depends on `IFoo` and `IBar`:

```c++
class ServiceA
{
public:
    ServiceA(std::shared_ptr<IFoo>& foo, std::shared_ptr<IBar>& bar);
[...]
}
```

As long as the service implements a static `Create()` method that matches the
constructor signature, the dependency injection will happen automatically. The 
static `Create()` method is needed because, as of C\++17, it's not possible to 
deduce the arguments to a constructor in C++.

```c++
[...]
public:
    static auto Create(std::shared_ptr<IFoo>& foo, std::shared_ptr<IBar>& bar)
    {
        return std::make_shared<ServiceA>(foo, bar);
    }
```

Now, the service can be added to the host:

```c++
host.RegisterService<ServiceA>();
```

During the registration, the arguments to `ServiceA::Create()` are automatically
determined, are retrieved from the internal registry, and passed down to a new 
instance of the service.

Later, consumers of the Service Host can retrieve fully constructed instances of 
any of the available services:

```c++
auto service = host.GetService<ServiceA>();
service->DoServiceWork();
```
