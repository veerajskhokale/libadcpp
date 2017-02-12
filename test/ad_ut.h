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

    AssertError(const AssertError& assertError) noexcept
        : mExp(assertError.mExp), mFile(assertError.mFile),
        mLine(assertError.mLine), mFunc(assertError.mFunc)
    {
    }

    AssertError(AssertError&& assertError) noexcept
        : mExp(std::move(assertError.mExp)), mFile(std::move(assertError.mFile)),
        mLine(std::move(assertError.mLine)), mFunc(std::move(assertError.mFunc))
    {
    }

    ~AssertError() noexcept
    {
    }

    template <class Func>
    AssertError(const std::string& exp,
        const std::string& file, Int line, Func&& func) noexcept
        :  mExp(exp), mFile(file), mLine(line), mFunc(std::forward<Func>(func))
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

    UnitTest(const UnitTest&) noexcept = delete;
    UnitTest(UnitTest&&) noexcept = delete;

    virtual ~UnitTest() noexcept
    {
    }

    virtual Void operator()() = 0;

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
                 "\n Reason      : Assertion Failed" << AD_RESET <<
                 "\n Expression  : " << assertError.getExp() <<
                 "\n File        : " << assertError.getFile() <<
                 "\n Line        : " << assertError.getLine();

            if (assertError.hasFunc()) {
                mStrm << "\n Message     : ";
                assertError.callFunc(mStrm);
            }
            mStrm << '\n';
            return;
        } catch (std::exception& error) {
            finish();
            mFail = 1;
            mStrm <<
                "\n Reason      : Caught std::exception" <<
                "\n what()      : " << error.what() << '\n';
            return;
        } catch (...) {
            finish();
            mFail = 1;
            mStrm << "\n Unknown Exception Caught\n";
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
    using UTConstIter   = std::vector<std::unique_ptr<
        UnitTest>>::const_iterator;
    using StackType     = std::vector<UTConstIter>;
    using OutputStream  = std::ostream;
    using ErrorStream   = std::ostream;

public:
    UTRunner() noexcept
        : mUt()
    {
    }

    UTRunner(const UTRunner&) noexcept = delete;
    UTRunner(UTRunner&&) noexcept = delete;
    
    ~UTRunner() noexcept
    {
    }

    static Void setOutputStream(OutputStream& ostrm) noexcept
    {
        getOutputStream() = &ostrm;
    }

    static Void setErrorStream(ErrorStream& estrm) noexcept
    {
        getErrorStream() = &estrm;
    }

    template <class UT, class... Args>
    Void add(const std::string& name, Args&&... args) noexcept
    {
        mUt.push_back(std::make_unique<UT>(std::forward<Args>(args)...));
        mUt.back()->setName(name);
    }

    Bool run() noexcept
    {
        OutputStream& ostrm = *getOutputStream();
        ErrorStream& estrm = *getErrorStream();
        Int cnt = 0, fcnt = 0;
        Double totTime = 0;
        const Int INDENT = 80;

        ostrm << newline;
    
        for (auto ut = mUt.begin(); ut != mUt.end(); ++ut) {
            getStack().push_back(ut);

            ostrm << newline << newline << AD_BLUE << 
                "[RUN] " << AD_RESET << getFullName(ut);

            ++cnt;
            (*ut)->run();
            std::chrono::duration<Double> duration =
                (*ut)->getEnd() - (*ut)->getStart();
            totTime += duration.count();

            ostrm << newline;

            if ((*ut)->isFailed()) {
                ++fcnt;
                ostrm << AD_RED;
            }
            else {
                ostrm << AD_GREEN;
            }

            ostrm << "    \\";
            for (Int i = getIndent() + 5; i < INDENT; ++i) {
                ostrm << '.';
            }

            if ((*ut)->isFailed()) {
                ostrm << " FAIL" << AD_RESET;
            } else {
                ostrm << " PASS" << AD_RESET;
            }

            ostrm << std::setw(5) << std::right <<
                " [" << std::setw(15) << std::right << duration.count() << "s]";

            if (std::distance(ut, mUt.end()) != 1) {
                getStack().pop_back();
            }
        }
        ostrm <<
            newline << newline << " Total  : " << cnt <<
            newline << " Failed : " << fcnt <<
            newline << " Time   : " << totTime << "s" <<
            newline << newline;

        if (fcnt) {
            estrm << AD_RED << " FAILED UNIT TESTS" << AD_RESET <<
                '\n' << AD_RED << " -----------------" << AD_RESET << "\n\n";

            for (auto ut = mUt.begin(); ut != mUt.end(); ++ut) {
                if ((*ut)->isFailed()) {
                    estrm << AD_RED << " [" << getFullName(ut) << "]" << AD_RESET <<
                        (*ut)->getInfo() << '\n' << std::endl;
                }
            }
            estrm << AD_RESET;
        }

        getStack().pop_back();
        if (getStack().empty()) {
            ostrm << std::endl;
        }

        return fcnt > 0 ? false : true;
    }

private:
    static OutputStream*& getOutputStream() noexcept
    {
        static OutputStream* ostrm = &std::cout;
        return ostrm;
    }

    static ErrorStream*& getErrorStream() noexcept
    {
        static ErrorStream* estrm = &std::cerr;
        return estrm;
    }

    static StackType& getStack() noexcept
    {
        static StackType utStack;
        return utStack;
    }

    static Int& getIndent() noexcept
    {
        static Int indent = 0;
        return indent;
    }

    std::string getFullName(UTConstIter utIter) noexcept
    {
        std::string fullName;
        for (const auto& x : getStack()) {
            fullName += "/";
            fullName += (*x)->getName();
        }
        return fullName;
    }

    static std::ostream& newline(std::ostream& strm) noexcept
    {
        strm << '\n';
        getIndent() = 0;
        if (getStack().size()) {
            for (Size i = 0; i < getStack().size() - 1; ++i) {
                strm << "|   ";
                getIndent() += 4;
            }
            strm << "|---";
            getIndent() += 4;
        }
        return strm;
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