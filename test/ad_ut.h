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

using StreamType = std::ostringstream;

class AssertError
    : public std::exception
{
public:
    AssertError() noexcept
        : mExp(), mFile(), mLine(-1), mFunc()
    {
    }

    AssertError(const std::string& exp,
        const std::string& file, Int line) noexcept
        : mExp(exp), mFile(file), mLine(line), mFunc()
    {
    }

    template <class FuncT>
    AssertError(const std::string& exp,
        const std::string& file, Int line, FuncT func) noexcept
        :  mExp(exp), mFile(file), mLine(line), mFunc(func)
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

    std::string getFile() const noexcept
    {
        return mFile;
    }

    Int getLine() const noexcept
    {
        return mLine;
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
    std::string                         mExp;
    std::string                         mFile;
    Int                                 mLine;
    std::function<Void(StreamType&)>    mFunc;

};

template <class Stream>
Stream& newline(Stream& strm);

std::ostream& UTnewline(std::ostream& strm)
{
    strm << newline<std::ostream>;
    return strm;
}

class UnitTest
{
    using TimePoint = std::chrono::
        time_point<std::chrono::high_resolution_clock>;

    friend class UTRunner;

public:
    UnitTest() noexcept
        : mName(), mExec(0), mStart(),
        mEnd(), mFail(0), mStrm()
    {
    }

    virtual Void operator()() = 0;

    virtual ~UnitTest() noexcept
    {
    }

private:
    Void setName(const std::string& name) noexcept
    {
        mName = name;
    }

    std::string getName() const noexcept
    {
        return mName;
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
                UTnewline << " Reason      : Assertion Failed" << AD_RESET <<
                UTnewline << " Expression  : " << assertError.getExp() <<
                UTnewline << " File        : " << assertError.getFile() <<
                UTnewline << " Line        : " << assertError.getLine();

            if (assertError.hasFunc()) {
                mStrm << UTnewline << " Message     : ";
                assertError.callFunc(mStrm);
            }
            mStrm << UTnewline;
            return;
        } catch (std::exception& error) {
            finish();
            mFail = 1;
            mStrm <<
                UTnewline << " Reason      : Caught std::exception" <<
                UTnewline << " what()      : " << error.what() << UTnewline;
            return;
        } catch (...) {
            finish();
            mFail = 1;
            mStrm << UTnewline << " Unknown Exception Caught" << UTnewline;
            return;
        }
        finish();
    }

    std::string                 mName;

    Bool                        mExec;
    TimePoint                   mStart;
    TimePoint                   mEnd;

    Bool                        mFail;
    std::ostringstream          mStrm;

};

class UTRunner
{
public:
    template <class Stream>
    friend Stream& newline(Stream& strm)
    {
        strm << '\n';
        UTRunner::getIndent() = 0;
        if (UTRunner::getStack().size()) {
            for (Size i = 0; i < UTRunner::getStack().size() - 1; ++i) {
                strm << "|   ";
                UTRunner::getIndent() += 4;
            }
            strm << "|---";
            UTRunner::getIndent() += 4;
        }
        return strm;
    }

    template <class UT>
    Void add(const std::string& name)
    {
        mUt.push_back(std::make_unique<UT>());
        mUt.back()->setName(name);
    }

    template <class Stream>
    Bool run(Stream& strm)
    {
        Int cnt = 0, fcnt = 0;
        Double totTime = 0;
        const Int INDENT = 80;

        strm << newline<Stream>;
    
        for (auto ut = mUt.begin(); ut != mUt.end(); ++ut) {
            getStack().push_back(ut);

            strm << newline<Stream> << newline<Stream> << AD_BLUE
                << "[RUN] " << AD_RESET << getFullName(ut);

            ++cnt;
            (*ut)->run();
            std::chrono::duration<Double> duration =
                (*ut)->getEnd() - (*ut)->getStart();
            totTime += duration.count();

            strm << newline<Stream>;

            if ((*ut)->isFailed()) {
                ++fcnt;
                strm << AD_RED;
            }
            else {
                strm << AD_GREEN;
            }

            strm << "    \\";
            for (Int i = getIndent() + 5; i < INDENT; ++i) {
                strm << '.';
            }

            if ((*ut)->isFailed()) {
                strm << " FAIL" << AD_RESET;
            } else {
                strm << " PASS" << AD_RESET;
            }

            strm << std::setw(5) << std::right <<
                " [" << std::setw(15) << std::right << duration.count() << "s]";

            if (std::distance(ut, mUt.end()) != 1) {
                getStack().pop_back();
            }
        }
        strm <<
            newline<Stream> << newline<Stream> << " Total  : " << cnt <<
            newline<Stream> << " Failed : " << fcnt <<
            newline<Stream> << " Time   : " << totTime << "s" <<
            newline<Stream> << newline<Stream>;

        if (fcnt) {
            strm << AD_RED << " FAILED UNIT TESTS" << AD_RESET <<
                newline<Stream> << AD_RED << " -----------------" << AD_RESET <<
                newline<Stream> << newline<Stream>;

            for (auto ut = mUt.begin(); ut != mUt.end(); ++ut) {
                if ((*ut)->isFailed()) {
                    strm << AD_RED << " [" << getFullName(ut) << "]" << AD_RESET <<
                        (*ut)->getInfo() << newline<Stream> << newline<Stream>;
                        strm.flush();
                }
            }
            strm << AD_RESET;
        }

        getStack().pop_back();
        if (getStack().empty()) {
            strm << std::endl;
        }

        return fcnt > 0 ? false : true;
    }

private:
    using UTConstIter = std::vector<std::unique_ptr<
        UnitTest>>::const_iterator;
    using StackType = std::vector<UTConstIter>;

    static StackType& getStack()
    {
        static StackType utStack;
        return utStack;
    }

    static Int& getIndent()
    {
        static Int indent = 0;
        return indent;
    }

    std::string getFullName(UTConstIter utIter)
    {
        std::string fullName;
        for (const auto& x : getStack()) {
            fullName += "/";
            fullName += (*x)->getName();
        }
        return fullName;
    }

    std::vector<std::unique_ptr<UnitTest>> mUt;

};

#define AD_UT_ASSERT1(exp) do { \
    if (!(exp)) { \
        throw ad::AssertError(#exp, __FILE__, __LINE__); \
    } \
} while (0)

#define AD_UT_ASSERT2(exp, func) do { \
    if (!(exp)) { \
        throw ad::AssertError(#exp, __FILE__, __LINE__, func); \
    } \
} while (0)

#define AD_GET_MACRO(_0, _1, _2, NAME, ...) NAME
#define AD_UT_ASSERT(...) AD_GET_MACRO(_0, __VA_ARGS__, AD_UT_ASSERT2, AD_UT_ASSERT1)(__VA_ARGS__)

}

#endif