
#include <cstdio>

#include "template_helpers.h"
#include "service_host.h"

struct IFoo : public IService
{
    virtual void DoFoo(int) = 0;
};

class Foo : public IFoo
{
    void DoFoo(int) override
    {
        printf("Foo::DoFoo\n");
    }
};

struct IBar : public IService
{
    virtual int DoBar(const char*) = 0;
};

class Bar : public IBar
{
    int DoBar(const char*) override
    {
        printf("Bar::DoBar\n");
        return -1;
    }
};

struct IZoo : public IService
{
    virtual bool DoZoo(float) = 0;
};

class Zoo : public IZoo
{
    bool DoZoo(float) override
    {
        printf("Zoo::DoZoo\n");
        return true;
    }
};

class ServiceA : public IService
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
