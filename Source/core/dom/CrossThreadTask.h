/*
 * Copyright (C) 2009-2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CrossThreadTask_h
#define CrossThreadTask_h

#include "core/dom/ExecutionContext.h"
#include "core/dom/ExecutionContextTask.h"
#include "platform/CrossThreadCopier.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/TypeTraits.h"

namespace blink {

// Traits for the CrossThreadTask. They are used to map a cross-thread-capable
// copied type to a CrossThreadTask constructor parameter type.
template<typename T> struct CrossThreadTaskTraits {
    typedef const T& ParamType;
};

// We can copy pointer-like values.
template<typename T> struct CrossThreadTaskTraits<T*> {
    typedef T* ParamType;
};
template<typename T> struct CrossThreadTaskTraits<PassRefPtr<T>> {
    typedef PassRefPtr<T> ParamType;
};
template<typename T> struct CrossThreadTaskTraits<PassOwnPtr<T>> {
    typedef PassOwnPtr<T> ParamType;
};
template<typename T> struct CrossThreadTaskTraits<RawPtr<T>> {
    typedef RawPtr<T> ParamType;
};

template<typename P1, typename MP1>
class CrossThreadTask1 : public ExecutionContextTask {
public:
    typedef void (*Method)(ExecutionContext*, MP1);
    typedef CrossThreadTask1<P1, MP1> CrossThreadTask;
    typedef typename CrossThreadTaskTraits<P1>::ParamType Param1;

    static PassOwnPtr<CrossThreadTask> create(Method method, Param1 parameter1)
    {
        return adoptPtr(new CrossThreadTask(method, parameter1));
    }

private:
    CrossThreadTask1(Method method, Param1 parameter1)
        : m_method(method)
        , m_parameter1(WTF::ParamStorageTraits<P1>::wrap(parameter1))
    {
    }

    virtual void performTask(ExecutionContext* context)
    {
        (*m_method)(context, WTF::ParamStorageTraits<P1>::unwrap(m_parameter1));
    }

private:
    Method m_method;
    typename WTF::ParamStorageTraits<P1>::StorageType m_parameter1;
};

template<typename P1, typename MP1, typename P2, typename MP2>
class CrossThreadTask2 : public ExecutionContextTask {
public:
    typedef void (*Method)(ExecutionContext*, MP1, MP2);
    typedef CrossThreadTask2<P1, MP1, P2, MP2> CrossThreadTask;
    typedef typename CrossThreadTaskTraits<P1>::ParamType Param1;
    typedef typename CrossThreadTaskTraits<P2>::ParamType Param2;

    static PassOwnPtr<CrossThreadTask> create(Method method, Param1 parameter1, Param2 parameter2)
    {
        return adoptPtr(new CrossThreadTask(method, parameter1, parameter2));
    }

private:
    CrossThreadTask2(Method method, Param1 parameter1, Param2 parameter2)
        : m_method(method)
        , m_parameter1(WTF::ParamStorageTraits<P1>::wrap(parameter1))
        , m_parameter2(WTF::ParamStorageTraits<P2>::wrap(parameter2))
    {
    }

    virtual void performTask(ExecutionContext* context)
    {
        (*m_method)(context,
            WTF::ParamStorageTraits<P1>::unwrap(m_parameter1),
            WTF::ParamStorageTraits<P2>::unwrap(m_parameter2));
    }

private:
    Method m_method;
    typename WTF::ParamStorageTraits<P1>::StorageType m_parameter1;
    typename WTF::ParamStorageTraits<P2>::StorageType m_parameter2;
};

template<typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3>
class CrossThreadTask3 : public ExecutionContextTask {
public:
    typedef void (*Method)(ExecutionContext*, MP1, MP2, MP3);
    typedef CrossThreadTask3<P1, MP1, P2, MP2, P3, MP3> CrossThreadTask;
    typedef typename CrossThreadTaskTraits<P1>::ParamType Param1;
    typedef typename CrossThreadTaskTraits<P2>::ParamType Param2;
    typedef typename CrossThreadTaskTraits<P3>::ParamType Param3;

    static PassOwnPtr<CrossThreadTask> create(Method method, Param1 parameter1, Param2 parameter2, Param3 parameter3)
    {
        return adoptPtr(new CrossThreadTask(method, parameter1, parameter2, parameter3));
    }

private:
    CrossThreadTask3(Method method, Param1 parameter1, Param2 parameter2, Param3 parameter3)
        : m_method(method)
        , m_parameter1(WTF::ParamStorageTraits<P1>::wrap(parameter1))
        , m_parameter2(WTF::ParamStorageTraits<P2>::wrap(parameter2))
        , m_parameter3(WTF::ParamStorageTraits<P3>::wrap(parameter3))
    {
    }

    virtual void performTask(ExecutionContext* context)
    {
        (*m_method)(context,
            WTF::ParamStorageTraits<P1>::unwrap(m_parameter1),
            WTF::ParamStorageTraits<P2>::unwrap(m_parameter2),
            WTF::ParamStorageTraits<P3>::unwrap(m_parameter3));
    }

private:
    Method m_method;
    typename WTF::ParamStorageTraits<P1>::StorageType m_parameter1;
    typename WTF::ParamStorageTraits<P2>::StorageType m_parameter2;
    typename WTF::ParamStorageTraits<P3>::StorageType m_parameter3;
};

template<typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3, typename P4, typename MP4>
class CrossThreadTask4 : public ExecutionContextTask {
public:
    typedef void (*Method)(ExecutionContext*, MP1, MP2, MP3, MP4);
    typedef CrossThreadTask4<P1, MP1, P2, MP2, P3, MP3, P4, MP4> CrossThreadTask;
    typedef typename CrossThreadTaskTraits<P1>::ParamType Param1;
    typedef typename CrossThreadTaskTraits<P2>::ParamType Param2;
    typedef typename CrossThreadTaskTraits<P3>::ParamType Param3;
    typedef typename CrossThreadTaskTraits<P4>::ParamType Param4;

    static PassOwnPtr<CrossThreadTask> create(Method method, Param1 parameter1, Param2 parameter2, Param3 parameter3, Param4 parameter4)
    {
        return adoptPtr(new CrossThreadTask(method, parameter1, parameter2, parameter3, parameter4));
    }

private:
    CrossThreadTask4(Method method, Param1 parameter1, Param2 parameter2, Param3 parameter3, Param4 parameter4)
        : m_method(method)
        , m_parameter1(WTF::ParamStorageTraits<P1>::wrap(parameter1))
        , m_parameter2(WTF::ParamStorageTraits<P2>::wrap(parameter2))
        , m_parameter3(WTF::ParamStorageTraits<P3>::wrap(parameter3))
        , m_parameter4(WTF::ParamStorageTraits<P4>::wrap(parameter4))
    {
    }

    virtual void performTask(ExecutionContext* context)
    {
        (*m_method)(context,
            WTF::ParamStorageTraits<P1>::unwrap(m_parameter1),
            WTF::ParamStorageTraits<P2>::unwrap(m_parameter2),
            WTF::ParamStorageTraits<P3>::unwrap(m_parameter3),
            WTF::ParamStorageTraits<P4>::unwrap(m_parameter4));
    }

private:
    Method m_method;
    typename WTF::ParamStorageTraits<P1>::StorageType m_parameter1;
    typename WTF::ParamStorageTraits<P2>::StorageType m_parameter2;
    typename WTF::ParamStorageTraits<P3>::StorageType m_parameter3;
    typename WTF::ParamStorageTraits<P4>::StorageType m_parameter4;
};

template<typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3, typename P4, typename MP4, typename P5, typename MP5>
class CrossThreadTask5 : public ExecutionContextTask {
public:
    typedef void (*Method)(ExecutionContext*, MP1, MP2, MP3, MP4, MP5);
    typedef CrossThreadTask5<P1, MP1, P2, MP2, P3, MP3, P4, MP4, P5, MP5> CrossThreadTask;
    typedef typename CrossThreadTaskTraits<P1>::ParamType Param1;
    typedef typename CrossThreadTaskTraits<P2>::ParamType Param2;
    typedef typename CrossThreadTaskTraits<P3>::ParamType Param3;
    typedef typename CrossThreadTaskTraits<P4>::ParamType Param4;
    typedef typename CrossThreadTaskTraits<P5>::ParamType Param5;

    static PassOwnPtr<CrossThreadTask> create(Method method, Param1 parameter1, Param2 parameter2, Param3 parameter3, Param4 parameter4, Param5 parameter5)
    {
        return adoptPtr(new CrossThreadTask(method, parameter1, parameter2, parameter3, parameter4, parameter5));
    }

private:
    CrossThreadTask5(Method method, Param1 parameter1, Param2 parameter2, Param3 parameter3, Param4 parameter4, Param5 parameter5)
        : m_method(method)
        , m_parameter1(WTF::ParamStorageTraits<P1>::wrap(parameter1))
        , m_parameter2(WTF::ParamStorageTraits<P2>::wrap(parameter2))
        , m_parameter3(WTF::ParamStorageTraits<P3>::wrap(parameter3))
        , m_parameter4(WTF::ParamStorageTraits<P4>::wrap(parameter4))
        , m_parameter5(WTF::ParamStorageTraits<P5>::wrap(parameter5))
    {
    }

    virtual void performTask(ExecutionContext* context)
    {
        (*m_method)(context,
            WTF::ParamStorageTraits<P1>::unwrap(m_parameter1),
            WTF::ParamStorageTraits<P2>::unwrap(m_parameter2),
            WTF::ParamStorageTraits<P3>::unwrap(m_parameter3),
            WTF::ParamStorageTraits<P4>::unwrap(m_parameter4),
            WTF::ParamStorageTraits<P5>::unwrap(m_parameter5));
    }

private:
    Method m_method;
    typename WTF::ParamStorageTraits<P1>::StorageType m_parameter1;
    typename WTF::ParamStorageTraits<P2>::StorageType m_parameter2;
    typename WTF::ParamStorageTraits<P3>::StorageType m_parameter3;
    typename WTF::ParamStorageTraits<P4>::StorageType m_parameter4;
    typename WTF::ParamStorageTraits<P5>::StorageType m_parameter5;
};

template<typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3, typename P4, typename MP4, typename P5, typename MP5, typename P6, typename MP6>
class CrossThreadTask6 : public ExecutionContextTask {
public:
    typedef void (*Method)(ExecutionContext*, MP1, MP2, MP3, MP4, MP5, MP6);
    typedef CrossThreadTask6<P1, MP1, P2, MP2, P3, MP3, P4, MP4, P5, MP5, P6, MP6> CrossThreadTask;
    typedef typename CrossThreadTaskTraits<P1>::ParamType Param1;
    typedef typename CrossThreadTaskTraits<P2>::ParamType Param2;
    typedef typename CrossThreadTaskTraits<P3>::ParamType Param3;
    typedef typename CrossThreadTaskTraits<P4>::ParamType Param4;
    typedef typename CrossThreadTaskTraits<P5>::ParamType Param5;
    typedef typename CrossThreadTaskTraits<P6>::ParamType Param6;

    static PassOwnPtr<CrossThreadTask> create(Method method, Param1 parameter1, Param2 parameter2, Param3 parameter3, Param4 parameter4, Param5 parameter5, Param6 parameter6)
    {
        return adoptPtr(new CrossThreadTask(method, parameter1, parameter2, parameter3, parameter4, parameter5, parameter6));
    }

private:
    CrossThreadTask6(Method method, Param1 parameter1, Param2 parameter2, Param3 parameter3, Param4 parameter4, Param5 parameter5, Param6 parameter6)
        : m_method(method)
        , m_parameter1(WTF::ParamStorageTraits<P1>::wrap(parameter1))
        , m_parameter2(WTF::ParamStorageTraits<P2>::wrap(parameter2))
        , m_parameter3(WTF::ParamStorageTraits<P3>::wrap(parameter3))
        , m_parameter4(WTF::ParamStorageTraits<P4>::wrap(parameter4))
        , m_parameter5(WTF::ParamStorageTraits<P5>::wrap(parameter5))
        , m_parameter6(WTF::ParamStorageTraits<P6>::wrap(parameter6))
    {
    }

    virtual void performTask(ExecutionContext* context)
    {
        (*m_method)(context,
            WTF::ParamStorageTraits<P1>::unwrap(m_parameter1),
            WTF::ParamStorageTraits<P2>::unwrap(m_parameter2),
            WTF::ParamStorageTraits<P3>::unwrap(m_parameter3),
            WTF::ParamStorageTraits<P4>::unwrap(m_parameter4),
            WTF::ParamStorageTraits<P5>::unwrap(m_parameter5),
            WTF::ParamStorageTraits<P6>::unwrap(m_parameter6));
    }

private:
    Method m_method;
    typename WTF::ParamStorageTraits<P1>::StorageType m_parameter1;
    typename WTF::ParamStorageTraits<P2>::StorageType m_parameter2;
    typename WTF::ParamStorageTraits<P3>::StorageType m_parameter3;
    typename WTF::ParamStorageTraits<P4>::StorageType m_parameter4;
    typename WTF::ParamStorageTraits<P5>::StorageType m_parameter5;
    typename WTF::ParamStorageTraits<P6>::StorageType m_parameter6;
};

template<typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3, typename P4, typename MP4, typename P5, typename MP5, typename P6, typename MP6, typename P7, typename MP7>
class CrossThreadTask7 : public ExecutionContextTask {
public:
    typedef void (*Method)(ExecutionContext*, MP1, MP2, MP3, MP4, MP5, MP6, MP7);
    typedef CrossThreadTask7<P1, MP1, P2, MP2, P3, MP3, P4, MP4, P5, MP5, P6, MP6, P7, MP7> CrossThreadTask;
    typedef typename CrossThreadTaskTraits<P1>::ParamType Param1;
    typedef typename CrossThreadTaskTraits<P2>::ParamType Param2;
    typedef typename CrossThreadTaskTraits<P3>::ParamType Param3;
    typedef typename CrossThreadTaskTraits<P4>::ParamType Param4;
    typedef typename CrossThreadTaskTraits<P5>::ParamType Param5;
    typedef typename CrossThreadTaskTraits<P6>::ParamType Param6;
    typedef typename CrossThreadTaskTraits<P7>::ParamType Param7;

    static PassOwnPtr<CrossThreadTask> create(Method method, Param1 parameter1, Param2 parameter2, Param3 parameter3, Param4 parameter4, Param5 parameter5, Param6 parameter6, Param7 parameter7)
    {
        return adoptPtr(new CrossThreadTask(method, parameter1, parameter2, parameter3, parameter4, parameter5, parameter6, parameter7));
    }

private:
    CrossThreadTask7(Method method, Param1 parameter1, Param2 parameter2, Param3 parameter3, Param4 parameter4, Param5 parameter5, Param6 parameter6, Param7 parameter7)
        : m_method(method)
        , m_parameter1(WTF::ParamStorageTraits<P1>::wrap(parameter1))
        , m_parameter2(WTF::ParamStorageTraits<P2>::wrap(parameter2))
        , m_parameter3(WTF::ParamStorageTraits<P3>::wrap(parameter3))
        , m_parameter4(WTF::ParamStorageTraits<P4>::wrap(parameter4))
        , m_parameter5(WTF::ParamStorageTraits<P5>::wrap(parameter5))
        , m_parameter6(WTF::ParamStorageTraits<P6>::wrap(parameter6))
        , m_parameter7(WTF::ParamStorageTraits<P7>::wrap(parameter7))
    {
    }

    virtual void performTask(ExecutionContext* context)
    {
        (*m_method)(context,
            WTF::ParamStorageTraits<P1>::unwrap(m_parameter1),
            WTF::ParamStorageTraits<P2>::unwrap(m_parameter2),
            WTF::ParamStorageTraits<P3>::unwrap(m_parameter3),
            WTF::ParamStorageTraits<P4>::unwrap(m_parameter4),
            WTF::ParamStorageTraits<P5>::unwrap(m_parameter5),
            WTF::ParamStorageTraits<P6>::unwrap(m_parameter6),
            WTF::ParamStorageTraits<P7>::unwrap(m_parameter7));
    }

private:
    Method m_method;
    typename WTF::ParamStorageTraits<P1>::StorageType m_parameter1;
    typename WTF::ParamStorageTraits<P2>::StorageType m_parameter2;
    typename WTF::ParamStorageTraits<P3>::StorageType m_parameter3;
    typename WTF::ParamStorageTraits<P4>::StorageType m_parameter4;
    typename WTF::ParamStorageTraits<P5>::StorageType m_parameter5;
    typename WTF::ParamStorageTraits<P6>::StorageType m_parameter6;
    typename WTF::ParamStorageTraits<P7>::StorageType m_parameter7;
};

template<typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3, typename P4, typename MP4, typename P5, typename MP5, typename P6, typename MP6, typename P7, typename MP7, typename P8, typename MP8>
class CrossThreadTask8 : public ExecutionContextTask {
public:
    typedef void (*Method)(ExecutionContext*, MP1, MP2, MP3, MP4, MP5, MP6, MP7, MP8);
    typedef CrossThreadTask8<P1, MP1, P2, MP2, P3, MP3, P4, MP4, P5, MP5, P6, MP6, P7, MP7, P8, MP8> CrossThreadTask;
    typedef typename CrossThreadTaskTraits<P1>::ParamType Param1;
    typedef typename CrossThreadTaskTraits<P2>::ParamType Param2;
    typedef typename CrossThreadTaskTraits<P3>::ParamType Param3;
    typedef typename CrossThreadTaskTraits<P4>::ParamType Param4;
    typedef typename CrossThreadTaskTraits<P5>::ParamType Param5;
    typedef typename CrossThreadTaskTraits<P6>::ParamType Param6;
    typedef typename CrossThreadTaskTraits<P7>::ParamType Param7;
    typedef typename CrossThreadTaskTraits<P8>::ParamType Param8;

    static PassOwnPtr<CrossThreadTask> create(Method method, Param1 parameter1, Param2 parameter2, Param3 parameter3, Param4 parameter4, Param5 parameter5, Param6 parameter6, Param7 parameter7, Param8 parameter8)
    {
        return adoptPtr(new CrossThreadTask(method, parameter1, parameter2, parameter3, parameter4, parameter5, parameter6, parameter7, parameter8));
    }

private:
    CrossThreadTask8(Method method, Param1 parameter1, Param2 parameter2, Param3 parameter3, Param4 parameter4, Param5 parameter5, Param6 parameter6, Param7 parameter7, Param8 parameter8)
        : m_method(method)
        , m_parameter1(WTF::ParamStorageTraits<P1>::wrap(parameter1))
        , m_parameter2(WTF::ParamStorageTraits<P2>::wrap(parameter2))
        , m_parameter3(WTF::ParamStorageTraits<P3>::wrap(parameter3))
        , m_parameter4(WTF::ParamStorageTraits<P4>::wrap(parameter4))
        , m_parameter5(WTF::ParamStorageTraits<P5>::wrap(parameter5))
        , m_parameter6(WTF::ParamStorageTraits<P6>::wrap(parameter6))
        , m_parameter7(WTF::ParamStorageTraits<P7>::wrap(parameter7))
        , m_parameter8(WTF::ParamStorageTraits<P8>::wrap(parameter8))
    {
    }

    virtual void performTask(ExecutionContext* context)
    {
        (*m_method)(context,
            WTF::ParamStorageTraits<P1>::unwrap(m_parameter1),
            WTF::ParamStorageTraits<P2>::unwrap(m_parameter2),
            WTF::ParamStorageTraits<P3>::unwrap(m_parameter3),
            WTF::ParamStorageTraits<P4>::unwrap(m_parameter4),
            WTF::ParamStorageTraits<P5>::unwrap(m_parameter5),
            WTF::ParamStorageTraits<P6>::unwrap(m_parameter6),
            WTF::ParamStorageTraits<P7>::unwrap(m_parameter7),
            WTF::ParamStorageTraits<P8>::unwrap(m_parameter8));
    }

private:
    Method m_method;
    typename WTF::ParamStorageTraits<P1>::StorageType m_parameter1;
    typename WTF::ParamStorageTraits<P2>::StorageType m_parameter2;
    typename WTF::ParamStorageTraits<P3>::StorageType m_parameter3;
    typename WTF::ParamStorageTraits<P4>::StorageType m_parameter4;
    typename WTF::ParamStorageTraits<P5>::StorageType m_parameter5;
    typename WTF::ParamStorageTraits<P6>::StorageType m_parameter6;
    typename WTF::ParamStorageTraits<P7>::StorageType m_parameter7;
    typename WTF::ParamStorageTraits<P8>::StorageType m_parameter8;
};

template<typename P1, typename MP1>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (*method)(ExecutionContext*, MP1),
    const P1& parameter1)
{
    return CrossThreadTask1<typename CrossThreadCopier<P1>::Type, MP1>::create(
        method,
        CrossThreadCopier<P1>::copy(parameter1));
}

template<typename P1, typename MP1, typename P2, typename MP2>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (*method)(ExecutionContext*, MP1, MP2),
    const P1& parameter1, const P2& parameter2)
{
    return CrossThreadTask2<typename CrossThreadCopier<P1>::Type, MP1, typename CrossThreadCopier<P2>::Type, MP2>::create(
        method,
        CrossThreadCopier<P1>::copy(parameter1), CrossThreadCopier<P2>::copy(parameter2));
}

template<typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (*method)(ExecutionContext*, MP1, MP2, MP3),
    const P1& parameter1, const P2& parameter2, const P3& parameter3)
{
    return CrossThreadTask3<typename CrossThreadCopier<P1>::Type, MP1, typename CrossThreadCopier<P2>::Type, MP2, typename CrossThreadCopier<P3>::Type, MP3>::create(
        method,
        CrossThreadCopier<P1>::copy(parameter1), CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3));
}

template<typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3, typename P4, typename MP4>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (*method)(ExecutionContext*, MP1, MP2, MP3, MP4),
    const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4)
{
    return CrossThreadTask4<typename CrossThreadCopier<P1>::Type, MP1, typename CrossThreadCopier<P2>::Type, MP2, typename CrossThreadCopier<P3>::Type, MP3,
        typename CrossThreadCopier<P4>::Type, MP4>::create(
            method,
            CrossThreadCopier<P1>::copy(parameter1), CrossThreadCopier<P2>::copy(parameter2),
            CrossThreadCopier<P3>::copy(parameter3), CrossThreadCopier<P4>::copy(parameter4));
}

template<typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3, typename P4, typename MP4, typename P5, typename MP5>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (*method)(ExecutionContext*, MP1, MP2, MP3, MP4, MP5),
    const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4, const P5& parameter5)
{
    return CrossThreadTask5<typename CrossThreadCopier<P1>::Type, MP1, typename CrossThreadCopier<P2>::Type, MP2, typename CrossThreadCopier<P3>::Type, MP3,
        typename CrossThreadCopier<P4>::Type, MP4, typename CrossThreadCopier<P5>::Type, MP5>::create(
            method,
            CrossThreadCopier<P1>::copy(parameter1), CrossThreadCopier<P2>::copy(parameter2),
            CrossThreadCopier<P3>::copy(parameter3), CrossThreadCopier<P4>::copy(parameter4),
            CrossThreadCopier<P5>::copy(parameter5));
}

template<typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3, typename P4, typename MP4, typename P5, typename MP5, typename P6, typename MP6>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (*method)(ExecutionContext*, MP1, MP2, MP3, MP4, MP5, MP6),
    const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4, const P5& parameter5, const P6& parameter6)
{
    return CrossThreadTask6<typename CrossThreadCopier<P1>::Type, MP1, typename CrossThreadCopier<P2>::Type, MP2, typename CrossThreadCopier<P3>::Type, MP3,
        typename CrossThreadCopier<P4>::Type, MP4, typename CrossThreadCopier<P5>::Type, MP5, typename CrossThreadCopier<P6>::Type, MP6>::create(
            method,
            CrossThreadCopier<P1>::copy(parameter1), CrossThreadCopier<P2>::copy(parameter2),
            CrossThreadCopier<P3>::copy(parameter3), CrossThreadCopier<P4>::copy(parameter4),
            CrossThreadCopier<P5>::copy(parameter5), CrossThreadCopier<P6>::copy(parameter6));
}

template<typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3, typename P4, typename MP4, typename P5, typename MP5, typename P6, typename MP6, typename P7, typename MP7>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (*method)(ExecutionContext*, MP1, MP2, MP3, MP4, MP5, MP6, MP7),
    const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4, const P5& parameter5, const P6& parameter6, const P7& parameter7)
{
    return CrossThreadTask7<typename CrossThreadCopier<P1>::Type, MP1, typename CrossThreadCopier<P2>::Type, MP2, typename CrossThreadCopier<P3>::Type, MP3,
        typename CrossThreadCopier<P4>::Type, MP4, typename CrossThreadCopier<P5>::Type, MP5, typename CrossThreadCopier<P6>::Type, MP6,
        typename CrossThreadCopier<P7>::Type, MP7>::create(
            method,
            CrossThreadCopier<P1>::copy(parameter1), CrossThreadCopier<P2>::copy(parameter2),
            CrossThreadCopier<P3>::copy(parameter3), CrossThreadCopier<P4>::copy(parameter4),
            CrossThreadCopier<P5>::copy(parameter5), CrossThreadCopier<P6>::copy(parameter6),
            CrossThreadCopier<P7>::copy(parameter7));
}

template<typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3, typename P4, typename MP4, typename P5, typename MP5, typename P6, typename MP6, typename P7, typename MP7, typename P8, typename MP8>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (*method)(ExecutionContext*, MP1, MP2, MP3, MP4, MP5, MP6, MP7, MP8),
    const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4, const P5& parameter5, const P6& parameter6, const P7& parameter7, const P8& parameter8)
{
    return CrossThreadTask8<typename CrossThreadCopier<P1>::Type, MP1, typename CrossThreadCopier<P2>::Type, MP2, typename CrossThreadCopier<P3>::Type, MP3,
    typename CrossThreadCopier<P4>::Type, MP4, typename CrossThreadCopier<P5>::Type, MP5, typename CrossThreadCopier<P6>::Type, MP6,
    typename CrossThreadCopier<P7>::Type, MP7, typename CrossThreadCopier<P8>::Type, MP8>::create(
                                                       method,
                                                       CrossThreadCopier<P1>::copy(parameter1), CrossThreadCopier<P2>::copy(parameter2),
                                                       CrossThreadCopier<P3>::copy(parameter3), CrossThreadCopier<P4>::copy(parameter4),
                                                       CrossThreadCopier<P5>::copy(parameter5), CrossThreadCopier<P6>::copy(parameter6),
                                                       CrossThreadCopier<P7>::copy(parameter7), CrossThreadCopier<P8>::copy(parameter8));
}

// createCrossThreadTask(...) is similar to but safer than
// CallClosureTask::create(bind(...)) for cross-thread task posting.
// postTask(CallClosureTask::create(bind(...))) is not thread-safe
// due to temporary objects, see http://crbug.com/390851 for details.
//
// createCrossThreadTask copies its arguments into Closure
// by CrossThreadCopier, rather than copy constructors.
// This means it creates deep copy of each argument if necessary.
//
// To pass things that cannot be copied by CrossThreadCopier
// (e.g. pointers), use AllowCrossThreadAccess() explicitly.
//
// If the first argument of createCrossThreadTask
// is a pointer to a member function in class C,
// then the second argument of createCrossThreadTask
// is a raw pointer (C*) or a weak pointer (const WeakPtr<C>&) to C.
// createCrossThreadTask does not use CrossThreadCopier for the pointer,
// assuming the user of createCrossThreadTask knows that the pointer
// can be accessed from the target thread.

// Templates for member function of class C + raw pointer (C*)
// which do not use CrossThreadCopier for the raw pointer (a1)
template<typename C>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (C::*function)(),
    C* p)
{
    return CallClosureTask::create(bind(function,
        p));
}

template<typename C, typename P1, typename MP1>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (C::*function)(MP1),
    C* p, const P1& parameter1)
{
    return CallClosureTask::create(bind(function,
        p,
        CrossThreadCopier<P1>::copy(parameter1)));
}

template<typename C, typename P1, typename MP1, typename P2, typename MP2>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (C::*function)(MP1, MP2),
    C* p, const P1& parameter1, const P2& parameter2)
{
    return CallClosureTask::create(bind(function,
        p,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2)));
}

template<typename C, typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (C::*function)(MP1, MP2, MP3),
    C* p, const P1& parameter1, const P2& parameter2, const P3& parameter3)
{
    return CallClosureTask::create(bind(function,
        p,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3)));
}

template<typename C, typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3, typename P4, typename MP4>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (C::*function)(MP1, MP2, MP3, MP4),
    C* p, const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4)
{
    return CallClosureTask::create(bind(function,
        p,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3),
        CrossThreadCopier<P4>::copy(parameter4)));
}

template<typename C, typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3, typename P4, typename MP4, typename P5, typename MP5>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (C::*function)(MP1, MP2, MP3, MP4, MP5),
    C* p, const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4, const P5& parameter5)
{
    return CallClosureTask::create(bind(function,
        p,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3),
        CrossThreadCopier<P4>::copy(parameter4),
        CrossThreadCopier<P5>::copy(parameter5)));
}

// Templates for member function of class C + weak pointer (const WeakPtr<C>&)
// which do not use CrossThreadCopier for the weak pointer (a1)
template<typename C>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (C::*function)(),
    const WeakPtr<C>& p)
{
    return CallClosureTask::create(bind(function,
        p));
}

template<typename C, typename P1, typename MP1>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (C::*function)(MP1),
    const WeakPtr<C>& p, const P1& parameter1)
{
    return CallClosureTask::create(bind(function,
        p,
        CrossThreadCopier<P1>::copy(parameter1)));
}

template<typename C, typename P1, typename MP1, typename P2, typename MP2>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (C::*function)(MP1, MP2),
    const WeakPtr<C>& p, const P1& parameter1, const P2& parameter2)
{
    return CallClosureTask::create(bind(function,
        p,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2)));
}

template<typename C, typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (C::*function)(MP1, MP2, MP3),
    const WeakPtr<C>& p, const P1& parameter1, const P2& parameter2, const P3& parameter3)
{
    return CallClosureTask::create(bind(function,
        p,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3)));
}

template<typename C, typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3, typename P4, typename MP4>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (C::*function)(MP1, MP2, MP3, MP4),
    const WeakPtr<C>& p, const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4)
{
    return CallClosureTask::create(bind(function,
        p,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3),
        CrossThreadCopier<P4>::copy(parameter4)));
}

template<typename C, typename P1, typename MP1, typename P2, typename MP2, typename P3, typename MP3, typename P4, typename MP4, typename P5, typename MP5>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    void (C::*function)(MP1, MP2, MP3, MP4, MP5),
    const WeakPtr<C>& p, const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4, const P5& parameter5)
{
    return CallClosureTask::create(bind(function,
        p,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3),
        CrossThreadCopier<P4>::copy(parameter4),
        CrossThreadCopier<P5>::copy(parameter5)));
}

// Other cases; use CrossThreadCopier for all arguments
template<typename FunctionType>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    FunctionType function)
{
    return CallClosureTask::create(bind(function));
}

template<typename FunctionType, typename P1>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    FunctionType function,
    const P1& parameter1)
{
    return CallClosureTask::create(bind(function,
        CrossThreadCopier<P1>::copy(parameter1)));
}

template<typename FunctionType, typename P1, typename P2>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    FunctionType function,
    const P1& parameter1, const P2& parameter2)
{
    return CallClosureTask::create(bind(function,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2)));
}

template<typename FunctionType, typename P1, typename P2, typename P3>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    FunctionType function,
    const P1& parameter1, const P2& parameter2, const P3& parameter3)
{
    return CallClosureTask::create(bind(function,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3)));
}

template<typename FunctionType, typename P1, typename P2, typename P3, typename P4>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    FunctionType function,
    const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4)
{
    return CallClosureTask::create(bind(function,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3),
        CrossThreadCopier<P4>::copy(parameter4)));
}

template<typename FunctionType, typename P1, typename P2, typename P3, typename P4, typename P5>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    FunctionType function,
    const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4, const P5& parameter5)
{
    return CallClosureTask::create(bind(function,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3),
        CrossThreadCopier<P4>::copy(parameter4),
        CrossThreadCopier<P5>::copy(parameter5)));
}

template<typename FunctionType, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
PassOwnPtr<ExecutionContextTask> createCrossThreadTask(
    FunctionType function,
    const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4, const P5& parameter5, const P6& parameter6)
{
    return CallClosureTask::create(bind(function,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3),
        CrossThreadCopier<P4>::copy(parameter4),
        CrossThreadCopier<P5>::copy(parameter5),
        CrossThreadCopier<P6>::copy(parameter6)));
}

} // namespace blink

#endif // CrossThreadTask_h
