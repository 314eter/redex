/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include <cstdint>
#include <cstdlib>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <string>
#include <array>

#include "DexClass.h"
#include "DexInstruction.h"
#include "DexLoader.h"
#include "Match.h"
#include "ProguardConfiguration.h"
#include "ProguardMap.h"
#include "ProguardMatcher.h"
#include "ProguardObfuscationTest.h"
#include "ProguardParser.h"
#include "ReachableClasses.h"
#include "RedexContext.h"

template<std::size_t SIZE>
void testClass(
    ProguardObfuscationTest* tester,
    const std::string& class_name,
    const std::array<std::string, SIZE>& fields) {
  auto clazz = tester->find_class_named(class_name);
  ASSERT_NE(nullptr, clazz) << class_name << " not found.";

  for (const std::string &fieldName : fields) {
    ASSERT_FALSE(tester->field_found(
        clazz->get_ifields(),
        class_name + fieldName)) << class_name + fieldName << " not obfuscated";
  }
}

/**
 * Check renaming has been properly applied.
 */
TEST(ProguardTest, obfuscation) {
  g_redex = new RedexContext();

  const char* dexfile = std::getenv("pg_config_e2e_dexfile");
  const char* mapping_file = std::getenv("pg_config_e2e_mapping");
  const char* configuration_file = std::getenv("pg_config_e2e_pgconfig");
  ASSERT_NE(nullptr, dexfile);
  ASSERT_NE(nullptr, configuration_file);

  ProguardObfuscationTest tester(dexfile, mapping_file);
  ASSERT_TRUE(tester.configure_proguard(configuration_file))
    << "Proguard configuration failed";

  const std::array<std::string, 3> implOneFields = {
    ".pubImplOneInt:I",
    ".pubImplOneString:Ljava/lang/String;",
    ".pubImplOneStringList:Ljava/util/List;"};
  const std::array<std::string, 2> theSuperFields = {
    ".pubSuperField:I",
    ".privSuperField:I" };
  const std::array<std::string, 2> subFields = {
    ".pubSubField:I",
    ".privSubField:I" };
  const std::array<std::string, 2> subImplFields = {
    ".pubSubImplField:I",
    ".privSubImplField:I" };
  const std::array<std::string, 3> subSubFields = {
    ".pubSubsubField:I",
    ".privSubsubField:I",
    ".privSuperField:I" };

  testClass(&tester,
    "Lcom/facebook/redex/test/proguard/ImplOne;",
    implOneFields);
  testClass(&tester,
    "Lcom/facebook/redex/test/proguard/TheSuper;",
    theSuperFields);
  testClass(&tester,
    "Lcom/facebook/redex/test/proguard/Sub;",
    subFields);
  testClass(&tester,
    "Lcom/facebook/redex/test/proguard/SubImpl;",
    subImplFields);
  testClass(&tester,
    "Lcom/facebook/redex/test/proguard/SubSub;",
    subSubFields);

  delete g_redex;
}
