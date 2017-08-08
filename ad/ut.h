/*
 * Copyright 2017 Veeraj S Khokale All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file    ad_ut.h
 * @brief   Unit test framework definition
 *
 * This file defines the libadcpp unit test framework. The unit test
 * framework is used throughout libadcpp for unit testing the code. The
 * framework is designed to be simple to use but at the same time it
 * provides powerful concepts such as hierarchical unit tests.
 *
 * From the user's point of view there are three main components in using
 * the unit test framework. They are the
 *  - class UnitTest
 *  - class UTRunner
 *  - macro AD_UT_ASSERT
 *
 * These components are explained in detail in the following documentation
 */

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

#include "ad/types.h"
#include "ad/util.h"

namespace ad
{

/**
 * @brief   Stream Object that is provided to AD_UT_ASSERT
 *
 * AD_UT_ASSERT is the macro used for assertions in the unit test
 * framework. See the documentation for this macro for further information
 * on this type.
 */
using StreamType = std::ostringstream;

/**
 * @brief   Type of the exception object thrown by AD_UT_ASSERT
 *
 * This class inherits from std::exception and is the type of the object
 * that is thrown by the assertion macro AD_UT_ASSERT when an assertion
 * fails. It is usally not required for the user of the unit test framework
 * to worry about this exception, it is handled by the framework internally.
 */
class AssertError
    : public std::exception
{
public:
    /**
     * @brief   Default constructor
     *
     * Initializes all member variables
     */
    AssertError()
        : mExp(), mFile(), mLine(-1), mFunc()
    {
    }

    /**
     * @brief   Construct an error object that describes the assertion error
     *
     * @param[in]   exp     The expression that evaluated to false thereby
     *                      causing the assertion to fail
     * @param[in]   file    The file in which the assertion failed.
     * @param[in]   line    The line in the file at which the assertion
     *                      failed
     *
     * The macro AD_UT_ASSERT fills exp, file and line automatically. In
     * fact a macro is used instead of a function for this very reason.
     */
    AssertError(const std::string& exp,
        const std::string& file, Int line)
        : mExp(exp), mFile(file), mLine(line), mFunc()
    {
    }

    /**
     * @brief   Construct an error object that describes the assertion error
     *
     * @tparam      Func    The type of the function object that can be
     *                      called for further describing the error. The
     *                      function object should have the signature
     *                      Void(StreamType&)
     *
     * @param[in]   exp     The expression that evaluated to false thereby
     *                      causing the assertion to fail
     * @param[in]   file    The file in which the assertion failed.
     * @param[in]   line    The line in the file at which the assertion
     *                      failed
     * @param[in]   func    The function that can be called to further
     *                      describe the error
     *
     * The macro AD_UT_ASSERT fills exp, file and line automatically. In
     * fact a macro is used instead of a function for this very reason.
     * To further facilitate the user to provide information on the error
     * that caused the assertion to fail, a function object can be provided
     * which will be called by the framework to print additional diagnostic
     * information. A stream is passed to this function so that information
     * can easily be written to it.
     */
    template <class Func>
    AssertError(const std::string& exp,
        const std::string& file, Int line, Func&& func)
        :  mExp(exp), mFile(file), mLine(line), mFunc(std::forward<Func>(func))
    {
    }

    /**
     * @brief   Copy constructor
     */
    AssertError(const AssertError& assertError)
        : mExp(assertError.mExp), mFile(assertError.mFile),
        mLine(assertError.mLine), mFunc(assertError.mFunc)
    {
    }

    /**
     * @brief   Move constructor
     */
    AssertError(AssertError&& assertError)
        : mExp(std::move(assertError.mExp)), mFile(std::move(assertError.mFile)),
        mLine(std::move(assertError.mLine)), mFunc(std::move(assertError.mFunc))
    {
    }

    /**
     * @brief   Destructor
     */
    ~AssertError()
    {
    }

    /**
     * @brief   Override for std::exception::what()
     *
     * @return  A description of the error
     */
    const Char* what() const noexcept
    {
        return mExp.c_str();
    }

    /**
     * @brief   Get the expression that caused the assertion to fail
     *
     * @return  The expression in the form of std::string
     */
    std::string getExp() const
    {
        return mExp;
    }

    /**
     * @brief   Get the file in which the assertion failed
     *
     * @return  The file in the form of std::string
     */
    std::string getFile() const
    {
        return mFile;
    }

    /**
     * @brief   Get the line in which the assertion failed
     *
     * @return  The line in the form of an integer
     */
    Int getLine() const
    {
        return mLine;
    }

    /**
     * @brief   Check if the error describing function is provided
     *
     * @return  True if the function is provided and false otherwise
     */
    Bool hasFunc() const
    {
        return static_cast<Bool>(mFunc);
    }

    /**
     * @brief   Call the error describing function
     *
     * @param[in]   strm    The stream object that should be passed on to
     *                      the error describing function
     */
    Void callFunc(StreamType& strm) const
    {
        mFunc(strm);
    }

private:
    std::string                         mExp;
    std::string                         mFile;
    Int                                 mLine;
    std::function<Void(StreamType&)>    mFunc;

}; /* class AssertError */

/**
 * @brief   The unit test class
 *
 * This is the base class from which every user defined unit test should
 * derive from. It is an abstract class and requires the operator () to be
 * overloaded by every deriving class. The user defines the unit test
 * in this operator () method which is called by the UTRunner to run the
 * unit test case.
 *
 * This implies that any class that derives from UnitTest and overrides
 * operator () can be a unit test case. The unit test may define other
 * methods as it deems fit, it can even have member variables and pretty
 * much anything that any other C++ class can have. The things to note
 * are that UnitTest can only be default constructed so the constructors
 * in the derived class should only call the default constructor of the
 * UnitTest class. Moreover the construction of unit test objects i.e.
 * classes that inherit from UnitTest is handled by the library. This
 * means that you tell the library the form of the unit test case as a
 * class and the library internally handles the construction and
 * execution of the test case. This prevents misuse by disallowing
 * multiple identical unit tests etc. For further details see the
 * documentation of UTRunner to see how the unit tests are given to the
 * library for execution.
 *
 * A unit test is considered PASS if and only if no assertions made using
 * AD_UT_ASSERT fail and no exceptions are thrown. In all other cases it
 * is considered to FAIL.
 */
class UnitTest
{
    using TimePoint = std::chrono::
        time_point<std::chrono::high_resolution_clock>;

    friend class UTRunner;

public:
    /**
     * @brief   Default constructor
     *
     * Always use this constructor from derived classes
     */
    UnitTest()
        : mName(), mExec(0), mStart(),
        mEnd(), mFail(0), mStrm()
    {
    }

    /**
     * @brief   Deleted copy constructor
     *
     * A unit test cannot be copied
     */
    UnitTest(const UnitTest&) = delete;

    /**
     * @brief   Deleted move constructor
     *
     * A unit test cannot be moved either since it is not necessary
     */
    UnitTest(UnitTest&&) = delete;

    /**
     * @brief   Destructor
     */
    virtual ~UnitTest()
    {
    }

    /**
     * @brief   operator ()
     *
     * To define a unit test inherit from this class and overide this
     * method. Note that any exceptions thrown from this method will
     * be handled by the library.
     */
    virtual Void operator()() = 0;

private:
    Void setName(const std::string& name)
    {
        mName = name;
    }

    std::string getName() const
    {
        return mName;
    }

    Bool isExecuted() const
    {
        return mExec;
    }

    TimePoint getStart() const
    {
        return mStart;
    }

    TimePoint getEnd() const
    {
        return mEnd;
    }

    Bool isFailed() const
    {
        return mFail;
    }

    std::string getInfo() const
    {
        return mStrm.str();
    }

    Void initialize()
    {
        mExec = mFail = 0;
        mStart = std::chrono::high_resolution_clock::now();
    }

    Void finish()
    {
        mExec = 1;
        mEnd = std::chrono::high_resolution_clock::now();
    }

    Void run()
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

/**
 * @brief   Unit test runner
 *
 * This is the class that is responsible for running the unit tests and
 * outputting the results. It is also the class that enables hierarchical
 * unit tests.
 *
 * First, you define the unit test cases as mentioned in UnitTest
 * documentation by inheriting from UnitTest and overriding operator ().
 * Once the unit tests are defined, you need to add them to a UTRunner
 * to execute them. You do this by using the UTRunner::add<>() method.
 * This is a template method and instead of passing a unit test object
 * you tell the add method the unit test class which derives from UnitTest
 * as a template parameter and UTRunner takes care of the rest. This is
 * to ensure that the unit tests are constructed by the library instead
 * of the user, preventing scenarios like duplicate tests etc. If the
 * unit test class that you defined needs parameters for construction,
 * you can pass it to the add method.
 *
 * Once you have added the desired unit tests to the UTRunner, you can
 * tell it to run the tests by calling the run() method. This method
 * executes each unit test added to this UTRunner by calling the operator ()
 * that you had overriden in your unit test class definition.
 *
 * You can set the streams for UTRunner to use for its output. You can set
 * the standard output stream and error stream to use. These are static
 * methods and hence are used by all UTRunner objects that you create.
 *
 * A very powerful feature that is supported by the library is the ability
 * to define unit test hierarchies. The UTRunner is the main class that
 * enables this capability. Suppose you have a scenario where A, B, C, D,
 * E, F are unit tests that you need to define such that the unit test C
 * itself needs to run A and B and some extra code, and say F needs to run
 * C, D and E. Here "needs" can imply a dependency or just a logical
 * hierarchy. Here is the required execution hierarchy:
 *
 * <pre>
 * |--C
 * |  |-- A
 * |  `-- B
 * `-- F
 *     |-- C
 *     |   |-- A *
 *     |   `-- B
 *     |-- D
 *     `-- E
 *
 * You can easily achieve such a hierarchy using UTRunner by using the
 * following:
 *
 * class A : public UnitTest { Void operator()() {...} };
 * class B : public UnitTest { Void operator()() {...} };
 *
 * class C : public UnitTest
 * {
 *     Void operator()()
 *     {
 *         ...
 *         UTRunner utRunner;
 *         utRunner.add<A>();
 *         utRunner.add<B>();
 *         AD_UT_ASSERT(utRunner.run());
 *     }
 * };
 *
 * class D : public UnitTest { Void operator()() {...} };
 * class E : public UnitTest { Void operator()() {...} };
 *
 * class F : public UnitTest
 * {
 *     Void operator()()
 *     {
 *         ...
 *         UTRunner utRunner;
 *         utRunner.add<C>();
 *         utRunner.add<D>();
 *         utRunner.add<E>();
 *         AD_UT_ASSERT(utRunner.run());
 *     }
 * };
 *
 * int main()
 * {
 *     UTRunner utRunner;
 *     utRunner.add<C>();
 *     utRunner.add<F>();
 *     utRunner.run();
 * }
 * </pre>
 *
 * This code produces the exact execution tree that is shown in the
 * figure above.
 *
 * As you can see complex hierarchies can be built like this without the
 * need for any additional requirements from the unit test code. Further
 * the output from UTRunner prints the output as shown in the above
 * figure, i.e. in the form of a tree with additional information. Infact
 * UTRunner automatically refects the hierarchy in the name of the unit
 * test that it uses for the output. For example in the above hiearchy,
 * while executing A (with * in the figure), it prints /F/C/A to denote
 * that it is executing the unit test with name A inside C inside F. This
 * makes it very easy to identify the failing tests in the hierarchy
 * since fail information is output to the error stream while the tree
 * is output to another stream. Also, such hierarchical test cases are
 * not just for getting nice outputs, it is also functional. For
 * example a run of the unit test C is considered to pass only if both
 * A and B pass, similarly F passes only if C (implies A and B pass), D
 * and E pass. This enables real hierarchical unit testing. It can also
 * be used for expressing dependencies among unit test cases.
 *
 * To see this sort of hierarchical testing in action execute the unit
 * tests for sorting algorithms. It uses a large hierarchy to test a
 * large number of scenarios for various sorting algorithms.
 */
class UTRunner
{
    using UTConstIter   = std::vector<std::unique_ptr<
        UnitTest>>::const_iterator;
    using StackType     = std::vector<UTConstIter>;
    using OutputStream  = std::ostream;
    using ErrorStream   = std::ostream;

public:
    /**
     * @brief   Default constructor
     */
    UTRunner()
        : mUt()
    {
    }

    /**
     * @brief   Deleted copy constructor
     */
    UTRunner(const UTRunner&) = delete;

    /**
     * @brief   Deleted move constructor
     */
    UTRunner(UTRunner&&) = delete;

    /**
     * @brief   Destructor
     */
    ~UTRunner()
    {
    }

    /**
     * @brief   Set the standard output stream
     *
     * @param   ostrm   The stream to use to standard output
     */
    static Void setOutputStream(OutputStream& ostrm)
    {
        getOutputStream() = &ostrm;
    }

    /**
     * @brief   Set the error output stream
     *
     * @param   ostrm   The stream to use to error output
     *
     * This stream contains information on failed unit test cases.
     */
    static Void setErrorStream(ErrorStream& estrm)
    {
        getErrorStream() = &estrm;
    }

    /**
     * @brief   Add a unit test to this runner
     *
     * @tparam  UT      The unit test class that defines this unit test
     *                  which has to be derived from UnitTest
     * @tparam  Args    Parameter pack for the arguments to pass to the
     *                  unit test
     *
     * @param   args    The arguments to pass to this unit test
     */
    template <class UT, class... Args>
    Void add(Args&&... args)
    {
        mUt.push_back(std::make_unique<UT>(std::forward<Args>(args)...));
        mUt.back()->setName(Name<UT>()());
    }

    /**
     * @brief  Run all unit tests added to this runner
     *
     * @return  True if all tests passed else false
     */
    Bool run()
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
    static OutputStream*& getOutputStream()
    {
        static OutputStream* ostrm = &std::cout;
        return ostrm;
    }

    static ErrorStream*& getErrorStream()
    {
        static ErrorStream* estrm = &std::cerr;
        return estrm;
    }

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

    static std::ostream& newline(std::ostream& strm)
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

/**
 * @brief   Assertion macro overload for 1 parameter
 *
 * @param   exp     The expression to evaluate
 *
 * If exp evaluates to false then the assertion FAILS.
 */
#define AD_UT_ASSERT1(exp) do { \
    if (!(exp)) { \
        throw ad::AssertError(#exp, __FILE__, __LINE__); \
    } \
} while (0)

/**
 * @brief   Assertion macro overload for 2 parameter
 *
 * @param   exp     The expression to evaluate
 * @param   func    The error describing function
 *
 * If exp evaluates to false then the assertion FAILS. When the assertion
 * fails func can be used to provide additional information. It should be
 * noted that func is not called immediately when an assertion fails. The
 * only thing that is guaranteed is that when it is called the unit test
 * that caused the assertion to fail still resides in memory. Therefore
 * this function can use member variables of the unit test class but it
 * cannot use local variables in the operator () function for example.
 */
#define AD_UT_ASSERT2(exp, func) do { \
    if (!(exp)) { \
        throw ad::AssertError(#exp, __FILE__, __LINE__, func); \
    } \
} while (0)

#define AD_GET_MACRO(_0, _1, _2, NAME, ...) NAME
#define AD_UT_ASSERT(...) AD_GET_MACRO(_0, __VA_ARGS__, AD_UT_ASSERT2, AD_UT_ASSERT1)(__VA_ARGS__)

}

#endif