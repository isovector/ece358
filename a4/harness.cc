#include "tests/harness.h"

#include <iostream>
#include <cstdlib>
#include <vector>
#include <errno.h>
#include <cstdio>
using namespace std;


typedef map<const char*, unit_test_func> tests_t;
tests_t UnitTestRegistrar::registeredTests;

ostream &set_color(bool) {
    return cout << "\033[0m";
}

ostream &set_color(const char *color) {
    return cout << "\033[" << color << "m";
}

bool testHasFailed;
void fail_test(const char *msg) {
    set_color("0;31") << "Expected `" << msg << "` failed" << endl;
    cout << "    errno: " << errno << endl;
    set_color(false);
    testHasFailed = true;
}

void fail_test_n(const char *msg, int val, int n) {
    set_color("0;31") << "Expected `" << msg << "` to have value `" << n << "`, got `" << val << "`"  << endl;
    cout << "    errno: " << errno << endl;
    perror("    perror");
    set_color(false);
    testHasFailed = true;
}

int main(int argc, char *argv[]) {
    srand(0);
    tests_t &regTests = UnitTestRegistrar::registeredTests;

    vector<const char*> failedTests;

    for (tests_t::const_iterator it = regTests.begin(); it != regTests.end(); ++it) {
        testHasFailed = false;
        cout << "Running test [";
            set_color("0;33") << it->first;
            set_color(false) << "]: " << endl;
        it->second();

        if (testHasFailed) {
            set_color("1;31") << "FAILED" << endl;
            failedTests.push_back(it->first);
        } else {
            set_color("1;32") << "PASSED" << endl;
        }
        set_color(false);

        cout << endl;
    }

    if (!failedTests.empty()) {
        set_color("1;31") << "The following tests FAILED:" << endl;
        set_color("0;33");
        for (size_t i = 0; i < failedTests.size(); ++i) {
            cout << "\t" << failedTests[i] << endl;
        }
    } else {
        set_color("0;32") << "All tests PASSED" << endl;
    }

    set_color(false);

    return failedTests.size();
}

