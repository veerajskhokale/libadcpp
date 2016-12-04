#ifndef AD_TEST_UT_H_
#define AD_TEST_UT_H_

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include <memory>
#include <utility>
#include "ad_types.h"

namespace ad
{

class Unit_test
{
    using Time_point = std::chrono::
        time_point<std::chrono::high_resolution_clock>;

public:
    Unit_test(const std::string& name, const std::string& file, int line)
        : m_name(name), m_file(file), m_line(line), m_exec(0), m_fail(0)
    {
    }

    virtual void operator()() = 0;

    std::string get_name() { return m_name; }
    std::string get_file() { return m_file; }
    Int get_line() { return m_line; }

    Bool is_executed() { return m_exec; }
    Time_point get_start() { return m_start; }
    Time_point get_end() { return m_end; }

    Bool is_failed() { return m_fail; }
    std::string get_info() { return m_strm.str(); }

    void initialize() 
    {
        m_exec = m_fail = 0;
        m_start = std::chrono::high_resolution_clock::now();
    }

    void finish()
    {
        m_exec = 1;
        m_end = std::chrono::high_resolution_clock::now();
    }

protected:
    const std::string           m_name;
    const std::string           m_file;
    const Int                   m_line;

    Bool                        m_exec;
    Time_point                  m_start;
    Time_point                  m_end;

    Bool                        m_fail;
    std::ostringstream          m_strm;

};

#define AD_UT_REGISTER(ut) \
public: \
    ut() \
        : Unit_test(#ut, __FILE__, __LINE__) \
    { \
    }

#define AD_UT_ASSERT1(exp) do { \
    if (!(exp)) { \
        this->finish(); \
        this->m_fail = 1; \
        this->m_strm << #exp << '\n'; \
    } \
} while (0)

#define AD_UT_ASSERT2(exp, func) do { \
    if (!(exp)) { \
        this->finish(); \
        this->m_fail = 1; \
        this->m_strm << #exp << '\n' << "MSG   : "; \
        func(this->m_strm); \
    } \
} while (0)

#define AD_GET_MACRO(_0, _1, _2, NAME, ...) NAME
#define AD_UT_ASSERT(...) AD_GET_MACRO(_0, __VA_ARGS__, AD_UT_ASSERT2, AD_UT_ASSERT1)(__VA_ARGS__)      

using Ut_runner = std::vector<std::unique_ptr<Unit_test>>;

template <class Ut>
void ut_add(Ut_runner& ut_runner)
{
    ut_runner.push_back(std::make_unique<Ut>());
}

template <class Forward_it, class Stream>
void ut_run(Forward_it first, Forward_it last, Stream& strm)
{
    const Int SLNO_WIDTH = 8;
    const Int NAME_WIDTH = 20;
    const Int PASS_WIDTH = 12;
    const Int TIME_WIDTH = 12;
    const Int FULL_WIDTH = SLNO_WIDTH + NAME_WIDTH + PASS_WIDTH + TIME_WIDTH;

    strm << std::left << "\n\n" <<
        std::setw(SLNO_WIDTH) << "SLNO" <<
        std::setw(NAME_WIDTH) << "NAME" <<
        std::setw(PASS_WIDTH) << "PASS/FAIL" <<
        std::setw(TIME_WIDTH) << "TIME(s)" << '\n';

    for (Int i = 0; i < FULL_WIDTH; ++i)
    {
        strm << '-';
    }
    strm << "\n\n";

    Int cnt = 0, fcnt = 0;
    for (auto ut = first; ut != last; ++ut)
    {
        ++cnt;
        (*ut)->operator()();
        std::chrono::duration<double> duration = (*ut)->get_end() - (*ut)->get_start();

        strm << std::left <<
            std::setw(SLNO_WIDTH) << cnt <<
            std::setw(NAME_WIDTH) << (*ut)->get_name();

        if ((*ut)->is_failed())
        {
            ++fcnt;
            strm << std::left <<
                AD_RED << std::setw(PASS_WIDTH) << "FAIL" << AD_RESET;
        }
        else
        {
            strm << std::left <<
                AD_GREEN << std::setw(PASS_WIDTH) << "PASS" << AD_RESET;
        }

        strm << std::left <<
            std::setw(TIME_WIDTH) << 
            duration.count() << '\n';
    }
    strm << "\n\nUnit Tests failed: " << fcnt << "\n\n";

    for (auto ut = first; ut != last; ++ut)
    {
        if ((*ut)->is_failed())
        {
            strm <<
                "NAME  : " << (*ut)->get_name() << '\n' <<
                "FILE  : " << (*ut)->get_file() << '\n' <<
                "LINE  : " << (*ut)->get_line() << '\n' <<
                "EXPR  : " << (*ut)->get_info() << '\n' << std::endl;
        }
    }
}

}

#endif