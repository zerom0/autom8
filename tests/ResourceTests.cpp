/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "Resources/Resource.h"

#include <coap/RestResponse.h>

// Dummy function as in main.cpp
void inputURIUpdated(Resource* resource, const std::string& propertyName, const std::string& oldValue, const std::string& newValue) {
}

TEST(Resource, read_EmptyResource) {
  Resource r(nullptr);

  EXPECT_EQ(0, r.read().size());
}

TEST(Resource, readProperty) {
  Resource r(nullptr);
  auto propName = std::string("aProperty");

  EXPECT_THROW(r.readProperty(propName), std::runtime_error);
}

TEST(Resource, readProperty_NonExistingPropertyThrows) {
  Resource r(nullptr);
  auto propName = std::string("aProperty");

  EXPECT_THROW(r.readProperty(propName), std::runtime_error);
}

TEST(Resource, createProperty_read) {
  Resource r(nullptr);
  auto propName = std::string("aProperty");

  EXPECT_THROW(r.readProperty(propName), std::runtime_error);

  Property p(false);
  r.createProperty(propName, p);

  ASSERT_EQ(1UL, r.read().size());
  EXPECT_EQ(propName, r.read().front());
}

TEST(Resource, createProperty_CreateTwiceThrows) {
  Resource r(nullptr);
  auto propName = std::string("aProperty");

  Property p(false);
  r.createProperty(propName, p);
  EXPECT_THROW(r.createProperty(propName, p), std::runtime_error);
}

TEST(Resource, deleteProperty) {
  Resource r(nullptr);
  auto propName = std::string("aProperty");

  Property p(false);
  r.createProperty(propName, p);

  ASSERT_EQ(1UL, r.read().size());

  r.deleteProperty(propName);

  EXPECT_EQ(0UL, r.read().size());
}

TEST(Resource, deleteProperty_DeleteUnknownThrows) {
  Resource r(nullptr);
  auto propName = std::string("aProperty");

  EXPECT_THROW(r.deleteProperty(propName), std::runtime_error);
}

TEST(Resource, updateProperty) {
  Resource r(nullptr);
  auto propName = std::string("aProperty");

  Property p(true);
  r.createProperty(propName, p);

  EXPECT_EQ("", r.readProperty(propName));

  r.updateProperty(propName, "new");

  EXPECT_EQ("new", r.readProperty(propName));
}

TEST(Resource, updateProperty_ReadOnlyCannotBeUpdated) {
  Resource r(nullptr);
  auto propName = std::string("aProperty");

  Property p(false); // isWriteable = false
  r.createProperty(propName, p);

  EXPECT_EQ("", r.readProperty(propName));

  EXPECT_THROW(r.updateProperty(propName, "new"), std::runtime_error);

  EXPECT_EQ("", r.readProperty(propName));
}

TEST(Resource, updateProperty_UpdateUnknownThrows) {
  Resource r(nullptr);
  auto propName = std::string("aProperty");

  EXPECT_THROW(r.updateProperty(propName, ""), std::runtime_error);
}

TEST(Resource, updateProperty_WithObserver) {
  Resource r(nullptr);
  auto propName = std::string("aProperty");

  std::string observedOldValue("not set");
  std::string observedNewValue("not set");
  auto observerCalled = 0UL;

  auto observer = [&observedOldValue, &observedNewValue, &observerCalled](const std::string& oldValue, const std::string& newValue){
    observedOldValue = oldValue;
    observedNewValue = newValue;
    ++observerCalled;
  };

  Property p(observer);
  r.createProperty(propName, p);

  r.updateProperty(propName, "new");

  EXPECT_EQ(1UL, observerCalled);
  EXPECT_EQ("", observedOldValue);
  EXPECT_EQ("new", observedNewValue);
}

TEST(Resource, updateProperty_WithResourceObserver) {
  std::string changedPropertyName("not set");
  auto observerCalled = 0UL;

  auto observer = [&changedPropertyName, &observerCalled](Resource* resource, const std::string& name){
    changedPropertyName = name;
    ++observerCalled;
  };

  Resource r(observer);
  auto propName = std::string("aProperty");

  Property p(true);
  r.createProperty(propName, p);

  r.updateProperty(propName, "new");

  EXPECT_EQ(1UL, observerCalled);
  EXPECT_EQ(propName, changedPropertyName);
}

TEST(Resource, subscribeProperty) {
  Resource r(nullptr);
  auto propName = std::string("aProperty");

  Property p(true);
  r.createProperty(propName, p);

  auto observerCalled = 0UL;
  std::string observedValue = "";

  auto notifications = std::make_shared<CoAP::Notifications>();
  notifications->subscribe([&observerCalled, &observedValue](const CoAP::RestResponse& notification){
    ++observerCalled;
    observedValue = notification.payload();
  });

  r.subscribeProperty(propName, notifications);
  EXPECT_EQ(0, observerCalled);
  EXPECT_EQ("", observedValue);

  r.updateProperty(propName, "new");
  EXPECT_EQ(1, observerCalled);
  EXPECT_EQ("new", observedValue);

  r.updateProperty(propName, "newer");
  EXPECT_EQ(2, observerCalled);
  EXPECT_EQ("newer", observedValue);
}

TEST(Resource, subscribeProperty_Twice) {
  Resource r(nullptr);
  auto propName = std::string("aProperty");

  Property p(true);
  r.createProperty(propName, p);

  auto observer1Called = 0UL;
  auto observer2Called = 0UL;
  std::string observedValue1 = "";
  std::string observedValue2 = "";

  auto notifications1 = std::make_shared<CoAP::Notifications>();
  notifications1->subscribe([&observer1Called, &observedValue1](const CoAP::RestResponse& notification){
    ++observer1Called;
    observedValue1 = notification.payload();
  });

  r.subscribeProperty(propName, notifications1);

  auto notifications2 = std::make_shared<CoAP::Notifications>();
  notifications2->subscribe([&observer2Called, &observedValue2](const CoAP::RestResponse& notification){
    ++observer2Called;
    observedValue2 = notification.payload();
  });

  r.subscribeProperty(propName, notifications2);

  EXPECT_EQ(0, observer1Called);
  EXPECT_EQ(0, observer2Called);
  EXPECT_EQ("", observedValue1);
  EXPECT_EQ("", observedValue2);

  r.updateProperty(propName, "new");

  EXPECT_EQ(1, observer1Called);
  EXPECT_EQ(1, observer2Called);
  EXPECT_EQ("new", observedValue1);
  EXPECT_EQ("new", observedValue2);
}

TEST(Resource, unsubscribeProperty) {
  Resource r(nullptr);
  auto propName = std::string("aProperty");

  Property p(true);
  r.createProperty(propName, p);

  auto observer1Called = 0UL;
  auto observer2Called = 0UL;
  std::string observedValue1 = "";
  std::string observedValue2 = "";

  auto notifications1 = std::make_shared<CoAP::Notifications>();
  notifications1->subscribe([&observer1Called, &observedValue1](const CoAP::RestResponse& notification){
    ++observer1Called;
    observedValue1 = notification.payload();
  });

  r.subscribeProperty(propName, notifications1);

  auto notifications2 = std::make_shared<CoAP::Notifications>();
  notifications2->subscribe([&observer2Called, &observedValue2](const CoAP::RestResponse& notification){
    ++observer2Called;
    observedValue2 = notification.payload();
  });

  r.subscribeProperty(propName, notifications2);

  EXPECT_EQ(0, observer1Called);
  EXPECT_EQ(0, observer2Called);
  EXPECT_EQ("", observedValue1);
  EXPECT_EQ("", observedValue2);

  r.updateProperty(propName, "new");

  EXPECT_EQ(1, observer1Called);
  EXPECT_EQ(1, observer2Called);
  EXPECT_EQ("new", observedValue1);
  EXPECT_EQ("new", observedValue2);

  notifications1.reset();
  r.updateProperty(propName, "newer");

  EXPECT_EQ(1, observer1Called);
  EXPECT_EQ(2, observer2Called);
  EXPECT_EQ("new", observedValue1);
  EXPECT_EQ("newer", observedValue2);
}