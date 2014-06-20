#pragma once

#define EXPECT(expr) { if (!(expr)) fail_test(#expr); }
#define EXPECT_N(n, expr) { int ___value = expr; if (___value != n) fail_test_n(#expr, ___value, n); }

#define UNIT_TEST(name) \
    void __unit_test__##name(); \
    UnitTestRegistrar __unit_test_registration__##name(\
        __FILE__ " >> " #name, \
        &__unit_test__##name); \
    void __unit_test__##name()
        

#include <map>

typedef void (*unit_test_func)();

class UnitTestRegistrar {
public:
    static std::map<const char*, unit_test_func> registeredTests;

    UnitTestRegistrar(const char *name, unit_test_func test) {
        registeredTests[name + 6] = test;
    }
};

void fail_test(const char *msg);
void fail_test_n(const char *msg, int value, int n);

