/****************************************************************************
**
** This file is part of the Ponder library, formerly CAMP.
**
** The MIT License (MIT)
**
** Copyright (C) 2009-2014 TEGESO/TEGESOFT and/or its subsidiary(-ies) and mother company.
** Contact: Tegesoft Information (contact@tegesoft.com)
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
** 
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
**
****************************************************************************/

#include <ponder/classget.hpp>
#include <ponder/property.hpp>
#include <ponder/pondertype.hpp>
#include <ponder/class.hpp>
#include <ponder/classbuilder.hpp>
#include <boost/test/unit_test.hpp>

namespace PropertyAccessTest
{
    struct MyClass
    {
        MyClass(bool b = true)
        : m_b(b)
        {
        }
        
        void set(int x) {p = x;}
        int get() const {return p;}
        int& ref() {return p;}
        int p;
        
        bool m_b;
        bool b1() {return true;}
        bool b2() const {return false;}
    };
    
    void declare()
    {
        using namespace std::placeholders;
        
        ponder::Class::declare<MyClass>("PropertyAccessTest::MyClass")
        
        // ***** constant value *****
        .property("p0", &MyClass::p).readable(false).writable(true)
        .property("p1", &MyClass::p).readable(true).writable(false)
        .property("p2", &MyClass::p).readable(false).writable(false)
        
        // ***** function *****
        .property("p3", &MyClass::p).readable(&MyClass::b1)
        .property("p4", &MyClass::p).readable(&MyClass::b2)
        .property("p5", &MyClass::p).readable(std::bind(&MyClass::b1, _1))
        .property("p6", &MyClass::p).readable(&MyClass::m_b)
        .property("p7", &MyClass::p).readable(std::function<bool (MyClass&)>(&MyClass::m_b))
        
        // ***** implicit - based on the availability of a getter/setter *****
        .property("p8",  &MyClass::get)
        .property("p9",  &MyClass::ref)
        .property("p10", &MyClass::get, &MyClass::set)
        ;
    }
}

PONDER_AUTO_TYPE(PropertyAccessTest::MyClass, &PropertyAccessTest::declare)

using namespace PropertyAccessTest;


struct PropertyAccessFixture
{
    PropertyAccessFixture()
        : object_t(true)
        , object_f(false)
    {
        metaclass = &ponder::classByType<MyClass>();
    }

    MyClass object_t;
    MyClass object_f;
    const ponder::Class* metaclass;
};

//-----------------------------------------------------------------------------
//                         Tests for ponder::Property readable/writable
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_SUITE(PROPERTYACCESS, PropertyAccessFixture)


BOOST_AUTO_TEST_CASE(readableImplicit)
{
    BOOST_CHECK_EQUAL(metaclass->property("p8").readable(object_t), true);
    BOOST_CHECK_EQUAL(metaclass->property("p8").readable(object_f), true);
    BOOST_CHECK_EQUAL(metaclass->property("p9").readable(object_t), true);
    BOOST_CHECK_EQUAL(metaclass->property("p9").readable(object_f), true);
    BOOST_CHECK_EQUAL(metaclass->property("p10").readable(object_t), true);
    BOOST_CHECK_EQUAL(metaclass->property("p10").readable(object_f), true);
}


BOOST_AUTO_TEST_CASE(readableStatic)
{
    BOOST_CHECK_EQUAL(metaclass->property("p0").readable(object_t), false);
    BOOST_CHECK_EQUAL(metaclass->property("p0").readable(object_f), false);
    BOOST_CHECK_EQUAL(metaclass->property("p1").readable(object_t), true);
    BOOST_CHECK_EQUAL(metaclass->property("p1").readable(object_f), true);
    BOOST_CHECK_EQUAL(metaclass->property("p2").readable(object_t), false);
    BOOST_CHECK_EQUAL(metaclass->property("p2").readable(object_f), false);
    BOOST_CHECK_EQUAL(metaclass->property("p3").readable(object_t), true);
    BOOST_CHECK_EQUAL(metaclass->property("p3").readable(object_f), true);
    BOOST_CHECK_EQUAL(metaclass->property("p4").readable(object_t), false);
    BOOST_CHECK_EQUAL(metaclass->property("p4").readable(object_f), false);
    BOOST_CHECK_EQUAL(metaclass->property("p5").readable(object_t), true);
    BOOST_CHECK_EQUAL(metaclass->property("p5").readable(object_f), true);
}


BOOST_AUTO_TEST_CASE(readableDynamic)
{
    BOOST_CHECK_EQUAL(metaclass->property("p6").readable(object_t), true);
    BOOST_CHECK_EQUAL(metaclass->property("p6").readable(object_f), false);
    BOOST_CHECK_EQUAL(metaclass->property("p7").readable(object_t), true);
    BOOST_CHECK_EQUAL(metaclass->property("p7").readable(object_f), false);
}


BOOST_AUTO_TEST_CASE(writableImplicit)
{
    BOOST_CHECK_EQUAL(metaclass->property("p8").writable(object_t), false);
    BOOST_CHECK_EQUAL(metaclass->property("p8").writable(object_f), false);
    BOOST_CHECK_EQUAL(metaclass->property("p9").writable(object_t), true);
    BOOST_CHECK_EQUAL(metaclass->property("p9").writable(object_f), true);
    BOOST_CHECK_EQUAL(metaclass->property("p10").writable(object_t), true);
    BOOST_CHECK_EQUAL(metaclass->property("p10").writable(object_f), true);
}


BOOST_AUTO_TEST_CASE(writableStatic)
{
    BOOST_CHECK_EQUAL(metaclass->property("p0").writable(object_t), true);
    BOOST_CHECK_EQUAL(metaclass->property("p0").writable(object_f), true);
    BOOST_CHECK_EQUAL(metaclass->property("p1").writable(object_t), false);
    BOOST_CHECK_EQUAL(metaclass->property("p1").writable(object_f), false);
    BOOST_CHECK_EQUAL(metaclass->property("p2").writable(object_t), false);
    BOOST_CHECK_EQUAL(metaclass->property("p2").writable(object_f), false);
}

BOOST_AUTO_TEST_SUITE_END()
