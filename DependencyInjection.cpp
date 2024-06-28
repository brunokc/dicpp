
#include <cstdio>

#include "template_helpers.h"
#include "service_host.h"

struct __declspec(uuid("6C51D48C-99C0-48EF-BD97-1D3870C9C738"))
IFoo : public IService
{
    virtual void DoFoo(int) = 0;
};

class Foo : public IFoo
{
    void DoFoo(int) override
    {
        printf("Foo::Foo\n");
    }
};

struct __declspec(uuid("BA0D80EB-7D5F-44B5-A48D-476F5DEA0192"))
IBar : public IService
{
    virtual int DoBar(const char*) = 0;
};

class Bar : public IBar
{
    int DoBar(const char*) override
    {
        printf("Bar::Bar\n");
        return -1;
    }
};

struct __declspec(uuid("B45603EA-1365-4504-9C32-675276237A69"))
IZoo : public IService
{
    virtual bool DoZoo(float) = 0;
};

class Zoo : public IZoo
{
    bool DoZoo(float) override
    {
        printf("Zoo::Zoo\n");
        return true;
    }
};

class __declspec(uuid("3327B3CD-AE0D-499A-9833-628FEF559513"))
ServiceA : public IService
{
public:
    ServiceA(const std::shared_ptr<IFoo>& foo, const std::shared_ptr<IBar>& bar) :
        _foo(foo),
        _bar(bar)
    {
    }

    static
    auto Create(const std::shared_ptr<IFoo>& foo, const std::shared_ptr<IBar>& bar)
    {
        return std::make_shared<ServiceA>(foo, bar);
    }

    void DoServiceWork()
    {
        _foo->DoFoo(42);
        int result = _bar->DoBar("the answer to everything");
        printf("ServiceA::DoServiceWork(): bar: %d\n", result);
    }

private:
    std::shared_ptr<IFoo> _foo;
    std::shared_ptr<IBar> _bar;
};

int main()
{
    ServiceHost host;

    host.RegisterService<IFoo, Foo>();
    host.RegisterService<IBar, Bar>();
    host.RegisterService<IZoo, Zoo>();
    host.RegisterService<ServiceA>();

    auto svc = host.GetService<ServiceA>();
    svc->DoServiceWork();

    return 0;
}
