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
#include <exception>
#include <functional>
#include "ad_types.h"

namespace ad
{

namespace internal
{

class Assert_error
    : public std::exception
{
public:
    Assert_error() noexcept
        : m_exp(), m_func()
    {
    }

    Assert_error(const std::string& exp) noexcept
        : m_exp(exp), m_func()
    {
    }

    template <class Func>
    Assert_error(const std::string& exp, Func func) noexcept
        : m_exp(exp), m_func(func) 
    {
    }

    const char* what() const noexcept
    {
        return m_exp.c_str();
    }

    std::string get_exp() const noexcept 
    { 
        return m_exp; 
    }

    Bool has_func() const noexcept 
    { 
        return static_cast<Bool>(m_func); 
    }
    
    void call_func(std::ostringstream& strm) const noexcept 
    {
        m_func(strm);
    }

private:
    std::string m_exp;
    std::function<void(std::ostringstream&)> m_func;

};

class Unit_test
{
    using Time_point = std::chrono::
        time_point<std::chrono::high_resolution_clock>;

    template <class Forward_it, class Stream>
    friend void ut_run(Forward_it first, Forward_it last, Stream& strm);

public:
    Unit_test(const std::string& name, 
        const std::string& file, int line) noexcept
        : m_name(name), m_file(file), m_line(line), m_exec(0), m_fail(0),
        m_start(), m_end(), m_strm() 
    {
    }

    virtual void operator()() = 0;

    virtual ~Unit_test() noexcept
    {
    }

private:
    std::string get_name() const noexcept 
    { 
        return m_name; 
    }
    
    std::string get_file() const  noexcept 
    { 
        return m_file; 
    }

    Int get_line() const noexcept 
    { 
        return m_line; 
    }

    Bool is_executed() const noexcept 
    { 
        return m_exec; 
    }

    Time_point get_start() const noexcept 
    { 
        return m_start; 
    }
    
    Time_point get_end() const noexcept 
    { 
        return m_end; 
    }

    Bool is_failed() const noexcept 
    { 
        return m_fail; 
    }
    
    std::string get_info() const noexcept 
    { 
        return m_strm.str(); 
    }

    void initialize() noexcept
    {
        m_exec = m_fail = 0;
        m_start = std::chrono::high_resolution_clock::now();
    }

    void finish() noexcept
    {
        m_exec = 1;
        m_end = std::chrono::high_resolution_clock::now();
    }

    void run() noexcept
    {
        initialize();
        try
        {
            this->operator()();
        }
        catch (Assert_error& assert_error)
        {
            finish();
            m_fail = 1;
            m_strm <<
                "\nAssertion Failed" <<
                "\nEXPR  : " << assert_error.get_exp();
            if (assert_error.has_func())
            {
                m_strm << "\nMSG   : ";
                assert_error.call_func(m_strm);
            }
            m_strm << '\n';
            return;
        }
        catch (std::exception& error)
        {
            finish();
            m_fail = 1;
            m_strm <<
                "\nException Caught" <<
                "\nWHAT  : " << error.what() << '\n';
            return;
        }
        catch(...)
        {
            finish();
            m_fail = 1;
            m_strm <<
                "\nException Caught" <<
                "\nWHAT  : UNKNOWN\n";
            return;
        }

        finish();
    }

    const std::string           m_name;
    const std::string           m_file;
    const Int                   m_line;

    Bool                        m_exec;
    Time_point                  m_start;
    Time_point                  m_end;

    Bool                        m_fail;
    std::ostringstream          m_strm;

};      

using Ut_runner = std::vector<std::unique_ptr<Unit_test>>;

template <class Ut>
void ut_add(Ut_runner& ut_runner)
{
    ut_runner.push_back(std::make_unique<Ut>());
}

template <class Forward_it, class Out_stream>
void ut_run(Forward_it first, Forward_it last, Out_stream& strm)
{
    Int cnt = 0, fcnt = 0;
    Double tot_time = 0;

    strm << "\n\n";
    
    for (auto ut = first; ut != last; ++ut)
    {
        ++cnt;
        (*ut)->run();
        std::chrono::duration<Double> duration = (*ut)->get_end() - (*ut)->get_start();
        tot_time += duration.count();

        strm << AD_BLUE << "[RUN] " << AD_RESET << (*ut)->get_name();

        if ((*ut)->is_failed())
        {
            ++fcnt;
            strm << AD_RED;
            
            Int length = (*ut)->get_name().size();
            Int num = (25 - length) > 0 ? (25 - length) : 0;
            
            strm << ' ';
            for (Int i = 0; i < num; ++i)
            {
                strm << '.';
            }
            strm << " FAIL" << AD_RESET;
        }
        else
        {
            strm << AD_GREEN;

            Int length = (*ut)->get_name().size();
            Int num = (25 - length) > 0 ? (25 - length) : 0;
            
            strm << ' ';
            for (Int i = 0; i < num; ++i)
            {
                strm << '.';
            }
            strm << " PASS" << AD_RESET;
        }

        strm << std::setw(30) << std::right <<
            " [" << std::setw(10) << std::right << duration.count() << "s] \n";
    }
    strm << 
        "\nTotal  : " << cnt <<
        "\nFailed : " << fcnt <<
        "\nTime   : " << tot_time << "s\n\n";

    strm << AD_RED << "FAILED UNIT TESTS\n-----------------\n";

    for (auto ut = first; ut != last; ++ut)
    {
        if ((*ut)->is_failed())
        {
            strm << AD_BLUE << "[" << (*ut)->get_name() << "] \n" << AD_RESET <<
                "FILE  : " << (*ut)->get_file() << '\n' <<
                "LINE  : " << (*ut)->get_line() << '\n' <<
                (*ut)->get_info() << '\n' << std::endl;
        }
    }
}

}

}

#define AD_UT(ut) \
class ut \
    : public ad::internal::Unit_test \
{ \
public: \
    ut() \
        : ad::internal::Unit_test(#ut, __FILE__, __LINE__) \
    { \
    } \
    \
    void operator()(); \
}; \
\
void ut::operator()() \

#define AD_UT_ASSERT1(exp) do { \
    if (!(exp)) \
    { \
        throw ad::internal::Assert_error(#exp); \
    } \
} while (0)

#define AD_UT_ASSERT2(exp, func) do { \
    if (!(exp)) \
    { \
        throw ad::internal::Assert_error(#exp, func); \
    } \
} while (0)

#define AD_GET_MACRO(_0, _1, _2, NAME, ...) NAME
#define AD_UT_ASSERT(...) AD_GET_MACRO(_0, __VA_ARGS__, AD_UT_ASSERT2, AD_UT_ASSERT1)(__VA_ARGS__)

#define AD_UT_RUNNER(ut_runner) ad::internal::Ut_runner ut_runner

#define AD_UT_ADD(ut_runner, ut) ad::internal::ut_add<ut>(ut_runner)

#define AD_UT_RUN(ut_runner, strm) ad::internal::ut_run(ut_runner.begin(), ut_runner.end(), strm)

#endif