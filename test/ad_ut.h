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

class AssertError
    : public std::exception
{
public:
    using StreamType = std::ostringstream;

    AssertError() noexcept
        : mExp(), mFunc()
    {
    }

    AssertError(const std::string& exp) noexcept
        : mExp(exp), mFunc()
    {
    }

    template <class FuncT>
    AssertError(const std::string& exp, FuncT func) noexcept
        :  mExp(exp), mFunc(func)
    {
    }

    const Char* what() const noexcept
    {
        return mExp.c_str();
    }

    std::string getExp() const noexcept
    {
        return mExp;
    }

    Bool hasFunc() const noexcept
    {
        return static_cast<Bool>(mFunc);
    }

    Void callFunc(StreamType& strm) const noexcept
    {
        mFunc(strm);
    }

private:
    std::string mExp;
    std::function<Void(StreamType&)> mFunc;

};

class UnitTest
{
    using TimePoint = std::chrono::
        time_point<std::chrono::high_resolution_clock>;

    template <class ForwardItT, class StreamT>
    friend Bool utRun(ForwardItT first, ForwardItT last, StreamT& strm);

public:
    UnitTest(const std::string& name, 
        const std::string& file, Int line) noexcept
        : mName(name), mFile(file), mLine(line), mExec(0), mFail(0),
        mStart(), mEnd(), mStrm()
    {
    }

    virtual Void operator()() = 0;

    virtual ~UnitTest() noexcept
    {
    }

private:
    std::string getName() const noexcept
    {
        return mName;
    }

    std::string getFile() const noexcept
    {
        return mFile;
    }

    Int getLine() const noexcept
    {
        return mLine;
    }

    Bool isExecuted() const noexcept
    {
        return mExec;
    }

    TimePoint getStart() const noexcept
    {
        return mStart;
    }

    TimePoint getEnd() const noexcept
    {
        return mEnd;
    }

    Bool isFailed() const noexcept
    {
        return mFail;
    }

    std::string getInfo() const noexcept
    {
        return mStrm.str();
    }

    Void initialize() noexcept
    {
        mExec = mFail = 0;
        mStart = std::chrono::high_resolution_clock::now();
    }

    Void finish() noexcept
    {
        mExec = 1;
        mEnd = std::chrono::high_resolution_clock::now();
    }

    Void run() noexcept
    {
        initialize();
        try {
            this->operator()();
        } catch (AssertError& assertError) {
            finish();
            mFail = 1;
            mStrm <<
                "\nAssertion Failed" <<
                "\nExpression  : " << assertError.getExp();

            if (assertError.hasFunc()) {
                mStrm << "\nMessage     : ";
                assertError.callFunc(mStrm);
            }
            mStrm << '\n';
            return;
        } catch (std::exception& error) {
            finish();
            mFail = 1;
            mStrm <<
                "\nCaught std::exception" <<
                "\nwhat()      : " << error.what() << '\n';
            return;
        } catch (...) {
            finish();
            mFail = 1;
            mStrm << "\nUnknown Exception Caught\n";
            return;
        }
        finish();
    }

    const std::string           mName;
    const std::string           mFile;
    const Int                   mLine;

    Bool                        mExec;
    TimePoint                   mStart;
    TimePoint                   mEnd;

    Bool                        mFail;
    std::ostringstream          mStrm;

};

using UTRunner = std::vector<std::unique_ptr<UnitTest>>;

template <class UT>
Void utAdd(UTRunner& utRunner)
{
    utRunner.push_back(std::make_unique<UT>());
}

template <class ForwardItT, class StreamT>
Bool utRun(ForwardItT first, ForwardItT last, StreamT& strm)
{
    Int cnt = 0, fcnt = 0;
    Double totTime = 0;
    const Int MAX_DOTS = 50;

    strm << "\n\n";
    
    for (auto ut = first; ut != last; ++ut) {
        ++cnt;
        (*ut)->run();
        std::chrono::duration<Double> duration = 
            (*ut)->getEnd() - (*ut)->getStart();
        totTime += duration.count();

        strm << AD_BLUE << "[RUN] " << AD_RESET << (*ut)->getName();

        if ((*ut)->isFailed()) {
            ++fcnt;
            strm << AD_RED;
            
            Int length = (*ut)->getName().size();
            Int num = (MAX_DOTS - length) > 0 ? (MAX_DOTS - length) : 0;
            
            strm << ' ';
            for (Int i = 0; i < num; ++i) {
                strm << '.';
            }
            strm << " FAIL" << AD_RESET;
        }
        else {
            strm << AD_GREEN;

            Int length = (*ut)->getName().size();
            Int num = (MAX_DOTS - length) > 0 ? (MAX_DOTS - length) : 0;
            
            strm << ' ';
            for (Int i = 0; i < num; ++i) {
                strm << '.';
            }
            strm << " PASS" << AD_RESET;
        }

        strm << std::setw(5) << std::right <<
            " [" << std::setw(15) << std::right << duration.count() << "s] \n";
    }
    strm << 
        "\nTotal  : " << cnt <<
        "\nFailed : " << fcnt <<
        "\nTime   : " << totTime << "s\n\n";

    if (fcnt) {
        strm << AD_RED << "FAILED UNIT TESTS\n-----------------\n";

        for (auto ut = first; ut != last; ++ut) {
            if ((*ut)->isFailed()) {
                strm << AD_BLUE << "[" << (*ut)->getName() << "] \n" << AD_RESET <<
                    "FILE  : " << (*ut)->getFile() << '\n' <<
                    "LINE  : " << (*ut)->getLine() << '\n' <<
                    (*ut)->getInfo() << '\n' << std::endl;
            }
        }
        strm << AD_RESET;
    }

    return fcnt > 0 ? false : true;
}

}
}

#define AD_UT_DECLARE(ut) \
class ut \
    : public ad::internal::UnitTest \
{ \
public: \
    ut(); \
    void operator()(); \
}

#define AD_UT_DEFINE(ut) \
ut::ut() \
    : ad::internal::UnitTest(#ut, __FILE__, __LINE__) \
{ \
} \
\
void ut::operator()() \

#define AD_UT(ut) \
AD_UT_DECLARE(ut); \
AD_UT_DEFINE(ut) \

#define AD_UT_ASSERT1(exp) do { \
    if (!(exp)) \
    { \
        throw ad::internal::AssertError(#exp); \
    } \
} while (0)

#define AD_UT_ASSERT2(exp, func) do { \
    if (!(exp)) \
    { \
        throw ad::internal::AssertError(#exp, func); \
    } \
} while (0)

#define AD_GET_MACRO(_0, _1, _2, NAME, ...) NAME
#define AD_UT_ASSERT(...) AD_GET_MACRO(_0, __VA_ARGS__, AD_UT_ASSERT2, AD_UT_ASSERT1)(__VA_ARGS__)

#define AD_UT_RUNNER(utRunner) ad::internal::UTRunner utRunner

#define AD_UT_ADD(utRunner, ut) ad::internal::utAdd<ut>(utRunner)

#define AD_UT_RUN(utRunner, strm) ad::internal::utRun(utRunner.begin(), utRunner.end(), strm)

#define AD_UT_STREAM_TYPE ad::internal::AssertError::StreamType

#endif