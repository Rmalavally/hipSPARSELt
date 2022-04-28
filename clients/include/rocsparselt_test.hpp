/* ************************************************************************
 * Copyright (c) 2018-2022 Advanced Micro Devices, Inc.
 * ************************************************************************ */

#pragma once

#ifdef GOOGLE_TEST
#include <gtest/gtest.h>
#endif

#include "../../library/src/include/handle.h"
#include "argument_model.hpp"
#include "rocsparselt.h"
#include "rocsparselt_arguments.hpp"
#include "test_cleanup.hpp"
#include <algorithm>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#ifdef GOOGLE_TEST
#define CHECK_SUCCESS(ERROR) ASSERT_EQ((ERROR), true)
// Extra macro so that macro arguments get expanded before calling Google Test
#define CHECK_HIP_ERROR2(ERROR) ASSERT_EQ(ERROR, hipSuccess)
#define CHECK_HIP_ERROR(ERROR) CHECK_HIP_ERROR2(ERROR)

#define CHECK_DEVICE_ALLOCATION(ERROR)                   \
    do                                                   \
    {                                                    \
        /* Use error__ in case ERROR contains "error" */ \
        hipError_t error__ = (ERROR);                    \
        if(error__ != hipSuccess)                        \
        {                                                \
            if(error__ == hipErrorOutOfMemory)           \
                SUCCEED() << LIMITED_MEMORY_STRING;      \
            else                                         \
                FAIL() << hipGetErrorString(error__);    \
            return;                                      \
        }                                                \
    } while(0)

// This wraps the rocSPARSELt call with catch_signals_and_exceptions_as_failures().
// By placing it at the rocSPARSELt call site, memory resources are less likely to
// be leaked in the event of a caught signal.
#define EXPECT_ROCSPARSELT_STATUS(STATUS, EXPECT)             \
    do                                                        \
    {                                                         \
        volatile bool signal_or_exception = true;             \
        /* Use status__ in case STATUS contains "status" */   \
        rocsparselt_status status__;                          \
        catch_signals_and_exceptions_as_failures([&] {        \
            status__            = (STATUS);                   \
            signal_or_exception = false;                      \
        });                                                   \
        if(signal_or_exception)                               \
            return;                                           \
        { /* localize status for ASSERT_EQ message */         \
            rocsparselt_status status_ = status__;            \
            ASSERT_EQ(status_, EXPECT); /* prints "status" */ \
        }                                                     \
    } while(0)

#else // GOOGLE_TEST

inline void rocsparselt_expect_status(rocsparselt_status status, rocsparselt_status expect)
{
    if(status != expect)
    {
        rocsparselt_cerr << "rocSPARSELt status error: Expected "
                         << rocsparselt_status_to_string(expect) << ", received "
                         << rocsparselt_status_to_string(status) << std::endl;
        if(expect == rocsparselt_status_success)
            exit(EXIT_FAILURE);
    }
}

#define CHECK_HIP_ERROR(ERROR)                                                             \
    do                                                                                     \
    {                                                                                      \
        /* Use error__ in case ERROR contains "error" */                                   \
        hipError_t error__ = (ERROR);                                                      \
        if(error__ != hipSuccess)                                                          \
        {                                                                                  \
            rocsparselt_cerr << "error: " << hipGetErrorString(error__) << " (" << error__ \
                             << ") at " __FILE__ ":" << __LINE__ << std::endl;             \
            exit(EXIT_FAILURE);                                                            \
        }                                                                                  \
    } while(0)

#define CHECK_DEVICE_ALLOCATION CHECK_HIP_ERROR

#define EXPECT_ROCSPARSELT_STATUS rocsparselt_expect_status

#define CHECK_SUCCESS(ERROR) \
    if(!(ERROR))             \
        exit(EXIT_FAILURE);

#endif // GOOGLE_TEST

#define CHECK_ROCSPARSELT_ERROR2(STATUS) \
    EXPECT_ROCSPARSELT_STATUS(STATUS, rocsparselt_status_success)
#define CHECK_ROCSPARSELT_ERROR(STATUS) CHECK_ROCSPARSELT_ERROR2(STATUS)

#ifdef GOOGLE_TEST

/* ============================================================================================ */
// Function which matches Arguments with a category, accounting for arg.known_bug_platforms
bool match_test_category(const Arguments& arg, const char* category);

// The tests are instantiated by filtering through the RocSparseLt_Data stream
// The filter is by category and by the type_filter() and function_filter()
// functions in the testclass
#define INSTANTIATE_TEST_CATEGORY(testclass, category)                           \
    INSTANTIATE_TEST_SUITE_P(                                                    \
        category,                                                                \
        testclass,                                                               \
        testing::ValuesIn(RocSparseLt_TestData::begin([](const Arguments& arg) { \
                              return match_test_category(arg, #category)         \
                                     && testclass::function_filter(arg)          \
                                     && testclass::type_filter(arg);             \
                          }),                                                    \
                          RocSparseLt_TestData::end()),                          \
        testclass::PrintToStringParamName());

#if !defined(WIN32) && defined(GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST)
#define ROCSPARSELT_ALLOW_UNINSTANTIATED_GTEST(testclass) \
    GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(testclass);
#else
#define ROCSPARSELT_ALLOW_UNINSTANTIATED_GTEST(testclass)
#endif

// Instantiate all test categories
#define INSTANTIATE_TEST_CATEGORIES(testclass)        \
    ROCSPARSELT_ALLOW_UNINSTANTIATED_GTEST(testclass) \
    INSTANTIATE_TEST_CATEGORY(testclass, _)

// Category based intantiation requires pass of large yaml data for each category
// Using single '_' named category and category name is moved to test name prefix
// gtest_filter should be able to select same test subsets
// INSTANTIATE_TEST_CATEGORY(testclass, quick)       \
// INSTANTIATE_TEST_CATEGORY(testclass, pre_checkin) \
// INSTANTIATE_TEST_CATEGORY(testclass, nightly)     \
// INSTANTIATE_TEST_CATEGORY(testclass, multi_gpu)   \
// INSTANTIATE_TEST_CATEGORY(testclass, HMM)         \
// INSTANTIATE_TEST_CATEGORY(testclass, known_bug)

// Function to catch signals and exceptions as failures
void catch_signals_and_exceptions_as_failures(std::function<void()> test, bool set_alarm = false);

// Macro to call catch_signals_and_exceptions_as_failures() with a lambda expression
#define CATCH_SIGNALS_AND_EXCEPTIONS_AS_FAILURES(test) \
    catch_signals_and_exceptions_as_failures([&] { test; }, true)

// Function to catch signals and exceptions as failures
void launch_test_on_threads(std::function<void()> test,
                            size_t                numThreads,
                            size_t                numStreams,
                            size_t                numDevices);

// Macro to call catch_signals_and_exceptions_as_failures() with a lambda expression
#define LAUNCH_TEST_ON_THREADS(test, threads, streams, devices) \
    launch_test_on_threads([&] { test; }, threads, streams, devices)

// Function to catch signals and exceptions as failures
void launch_test_on_streams(std::function<void()> test, size_t numStreams, size_t numDevices);

// Macro to call catch_signals_and_exceptions_as_failures() with a lambda expression
#define LAUNCH_TEST_ON_STREAMS(test, streams, devices) \
    launch_test_on_streams([&] { test; }, streams, devices)

// Macro to run test across threads
#define RUN_TEST_ON_THREADS_STREAMS(test)                                                    \
    do                                                                                       \
    {                                                                                        \
        const auto& arg          = GetParam();                                               \
        size_t      threads      = arg.threads;                                              \
        size_t      streams      = arg.streams;                                              \
        size_t      devices      = arg.devices;                                              \
        int         availDevices = 0;                                                        \
        bool        HMM          = arg.HMM;                                                  \
        hipGetDeviceCount(&availDevices);                                                    \
        if(devices > availDevices)                                                           \
        {                                                                                    \
            SUCCEED() << TOO_MANY_DEVICES_STRING;                                            \
            return;                                                                          \
        }                                                                                    \
        else if(HMM)                                                                         \
        {                                                                                    \
            for(int i = 0; i < devices; i++)                                                 \
            {                                                                                \
                int flag = 0;                                                                \
                CHECK_HIP_ERROR(hipDeviceGetAttribute(                                       \
                    &flag, hipDeviceAttribute_t(hipDeviceAttributeManagedMemory), devices)); \
                if(!flag)                                                                    \
                {                                                                            \
                    SUCCEED() << HMM_NOT_SUPPORTED;                                          \
                    return;                                                                  \
                }                                                                            \
            }                                                                                \
        }                                                                                    \
        g_stream_pool.reset(devices, streams);                                               \
        if(threads)                                                                          \
            LAUNCH_TEST_ON_THREADS(test, threads, streams, devices);                         \
        else                                                                                 \
            LAUNCH_TEST_ON_STREAMS(test, streams, devices);                                  \
    } while(0)

// Thread worker class
class thread_pool
{
    std::atomic_bool                                                 m_done{false};
    std::queue<std::pair<std::function<void()>, std::promise<void>>> m_work_queue;
    std::vector<std::thread>                                         m_threads;
    std::mutex                                                       m_mutex;
    std::condition_variable                                          m_cond;

    void worker_thread();

public:
    thread_pool();
    ~thread_pool();
    void submit(std::function<void()> func, std::promise<void> promise);
};

class stream_pool
{
    std::vector<std::vector<hipStream_t>> m_streams;

public:
    stream_pool() = default;

    void reset(size_t numDevices = 0, size_t numStreams = 0);

    ~stream_pool()
    {
        reset();
    }

    auto& operator[](size_t deviceId)
    {
        return m_streams.at(deviceId);
    }
};

extern stream_pool g_stream_pool;
extern thread_pool g_thread_pool;

extern thread_local std::unique_ptr<std::function<void(rocsparselt_handle)>> t_set_stream_callback;

/* ============================================================================================ */
/*! \brief  Normalized test name to conform to Google Tests */
// The template parameter is only used to generate multiple instantiations with distinct static local variables
template <typename>
class RocSparseLt_TestName
{
    std::ostringstream m_str;

public:
    explicit RocSparseLt_TestName(const char* name)
    {
        m_str << name << '_';
    }

    // Convert stream to normalized Google Test name
    // rvalue reference qualified so that it can only be called once
    // The name should only be generated once before the stream is destroyed
    operator std::string() &&
    {
        // This table is private to each instantation of RocSparseLt_TestName
        // Placed inside function to avoid dependency on initialization order
        static std::unordered_map<std::string, size_t>* table = test_cleanup::allocate(&table);
        std::string RocSparseLt_TestName_to_string(std::unordered_map<std::string, size_t>&,
                                                   const std::ostringstream&);
        return RocSparseLt_TestName_to_string(*table, m_str);
    }

    // Stream output operations
    template <typename U> // Lvalue LHS
    friend RocSparseLt_TestName& operator<<(RocSparseLt_TestName& name, U&& obj)
    {
        name.m_str << std::forward<U>(obj);
        return name;
    }

    template <typename U> // Rvalue LHS
    friend RocSparseLt_TestName&& operator<<(RocSparseLt_TestName&& name, U&& obj)
    {
        name.m_str << std::forward<U>(obj);
        return std::move(name);
    }

    RocSparseLt_TestName()                            = default;
    RocSparseLt_TestName(const RocSparseLt_TestName&) = delete;
    RocSparseLt_TestName& operator=(const RocSparseLt_TestName&) = delete;
};

// ----------------------------------------------------------------------------
// RocSparseLt_Test base class. All non-legacy rocSPARSELt Google tests derive from it.
// It defines a type_filter_functor() and a PrintToStringParamName class
// which calls name_suffix() in the derived class to form the test name suffix.
// ----------------------------------------------------------------------------
template <typename TEST, template <typename...> class FILTER>
class RocSparseLt_Test : public testing::TestWithParam<Arguments>
{
protected:
    // This template functor returns true if the type arguments are valid.
    // It converts a FILTER specialization to bool to test type matching.
    template <typename... T>
    struct type_filter_functor
    {
        bool operator()(const Arguments&)
        {
            return static_cast<bool>(FILTER<T...>{});
        }
    };

public:
    // Wrapper functor class which calls name_suffix()
    struct PrintToStringParamName
    {
        std::string operator()(const testing::TestParamInfo<Arguments>& info) const
        {
            std::string name(info.param.category);
            name += "_";
            name += TEST::name_suffix(info.param);
            return name;
        }
    };
};

// Function to set up signal handlers
void rocsparselt_test_sigaction();

#endif // GOOGLE_TEST

// ----------------------------------------------------------------------------
// Normal tests which return true when converted to bool
// ----------------------------------------------------------------------------
struct rocsparselt_test_valid
{
    // Return true to indicate the type combination is valid, for filtering
    virtual explicit operator bool() final
    {
        return true;
    }

    // Require derived class to define functor which takes (const Arguments &)
    virtual void operator()(const Arguments&) = 0;

    virtual ~rocsparselt_test_valid() = default;
};

// ----------------------------------------------------------------------------
// Error case which returns false when converted to bool. A void specialization
// of the FILTER class template above, should be derived from this class, in
// order to indicate that the type combination is invalid.
// ----------------------------------------------------------------------------
struct rocsparselt_test_invalid
{
    // Return false to indicate the type combination is invalid, for filtering
    virtual explicit operator bool() final
    {
        return false;
    }

    // If this specialization is actually called, print fatal error message
    virtual void operator()(const Arguments& arg) final
    {
        static constexpr char msg[] = "Internal error: Test called with invalid types";

#ifdef GOOGLE_TEST
        FAIL() << msg;
#else
        rocsparselt_cerr << msg << std::endl;
        rocsparselt_cerr << "function: " << arg.function << " types: "
                         << " a: " << rocsparselt_datatype2string(arg.a_type)
                         << " b: " << rocsparselt_datatype2string(arg.b_type)
                         << " c: " << rocsparselt_datatype2string(arg.c_type)
                         << " d: " << rocsparselt_datatype2string(arg.d_type)
                         << " compute:" << rocsparselt_computetype2string(arg.compute_type)
                         << std::endl;
        rocsparselt_abort();
#endif
    }

    virtual ~rocsparselt_test_invalid() = default;
};
