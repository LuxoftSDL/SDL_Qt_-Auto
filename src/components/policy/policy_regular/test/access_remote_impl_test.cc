/*
 * Copyright (c) 2015, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "gtest/gtest.h"
#include "policy/access_remote_impl.h"

namespace policy {

TEST(AccessRemoteImplTest, Allow) {
  AccessRemoteImpl access_remote;
  Subject who = {"dev1", "12345"};
  Object what = {policy_table::MT_RADIO};
  access_remote.Allow(who, what);
  AccessRemoteImpl::AccessControlList::const_iterator i =
      access_remote.acl_.find(what);
  ASSERT_NE(access_remote.acl_.end(), i);
  AccessRemoteImpl::AccessControlRow::const_iterator j = i->second.find(who);
  ASSERT_NE(i->second.end(), j);
  EXPECT_EQ(TypeAccess::kAllowed, j->second);
}

TEST(AccessRemoteImplTest, KeyMapTest) {
  // Testing operator < to use as key of map
  AccessRemoteImpl access_remote;
  Subject who = {"dev1", "12345"};
  Object what1 = {policy_table::MT_RADIO};
  Object what2 = {policy_table::MT_CLIMATE};
  access_remote.Allow(who, what1);
  access_remote.Allow(who, what2);
  ASSERT_EQ(2u, access_remote.acl_.size());
}

TEST(AccessRemoteImplTest, Deny) {
  AccessRemoteImpl access_remote;
  Subject who = {"dev1", "12345"};
  Object what = {policy_table::MT_RADIO};
  access_remote.Deny(who, what);
  AccessRemoteImpl::AccessControlList::const_iterator i =
      access_remote.acl_.find(what);
  ASSERT_NE(access_remote.acl_.end(), i);
  AccessRemoteImpl::AccessControlRow::const_iterator j = i->second.find(who);
  ASSERT_NE(i->second.end(), j);
  EXPECT_EQ(TypeAccess::kDisallowed, j->second);
}

TEST(AccessRemoteImplTest, ChangeAccess) {
  AccessRemoteImpl access_remote;
  Subject who = {"dev1", "12345"};
  Object what = {policy_table::MT_RADIO};
  access_remote.Allow(who, what);
  ASSERT_EQ(TypeAccess::kAllowed, access_remote.acl_[what][who]);
  access_remote.Deny(who, what);
  ASSERT_EQ(TypeAccess::kDisallowed, access_remote.acl_[what][who]);
  access_remote.Allow(who, what);
  EXPECT_EQ(TypeAccess::kAllowed, access_remote.acl_[what][who]);
}

TEST(AccessRemoteImplTest, ResetBySubject) {
  AccessRemoteImpl access_remote;
  Subject who = {"dev1", "12345"};
  Object what1 = {policy_table::MT_RADIO};
  Object what2 = {policy_table::MT_CLIMATE};
  access_remote.Allow(who, what1);
  access_remote.Deny(who, what2);
  ASSERT_EQ(2u, access_remote.acl_.size());
  ASSERT_EQ(1u, access_remote.acl_[what1].size());
  ASSERT_EQ(1u, access_remote.acl_[what2].size());

  access_remote.Reset(who);
  ASSERT_EQ(2u, access_remote.acl_.size());
  EXPECT_TRUE(access_remote.acl_[what1].empty());
  EXPECT_TRUE(access_remote.acl_[what2].empty());
}

TEST(AccessRemoteImplTest, ResetByObject) {
  AccessRemoteImpl access_remote;
  Subject who1 = {"dev1", "12345"};
  Subject who2 = {"dev2", "123456"};
  Object what = {policy_table::MT_RADIO};
  access_remote.Allow(who1, what);
  access_remote.Deny(who2, what);
  ASSERT_EQ(1u, access_remote.acl_.size());
  ASSERT_EQ(2u, access_remote.acl_[what].size());

  access_remote.Reset(what);
  EXPECT_TRUE(access_remote.acl_.empty());
}

TEST(AccessRemoteImplTest, CheckAllowed) {
  AccessRemoteImpl access_remote;
  Subject who = {"dev1", "12345"};
  Object what = {policy_table::MT_RADIO};
  access_remote.Allow(who, what);

  EXPECT_EQ(TypeAccess::kAllowed, access_remote.Check(who, what));
}

TEST(AccessRemoteImplTest, CheckDisallowed) {
  AccessRemoteImpl access_remote;
  Subject who = {"dev1", "12345"};
  Subject who1 = {"dev1", "123456"};
  Object what = {policy_table::MT_RADIO};

  access_remote.Allow(who, what);
  EXPECT_EQ(TypeAccess::kManual, access_remote.Check(who1, what));

  access_remote.Reset(who);
  access_remote.Deny(who1, what);
  EXPECT_EQ(TypeAccess::kDisallowed, access_remote.Check(who1, what));
}

TEST(AccessRemoteImplTest, CheckManual) {
  AccessRemoteImpl access_remote;
  Subject who = {"dev1", "12345"};
  Subject who1 = {"dev1", "123456"};
  Object what = {policy_table::MT_RADIO};

  EXPECT_EQ(TypeAccess::kManual, access_remote.Check(who, what));

  access_remote.Deny(who1, what);
  EXPECT_EQ(TypeAccess::kManual, access_remote.Check(who, what));
}

TEST(AccessRemoteImplTest, CheckModuleType) {
  AccessRemoteImpl access_remote;
  access_remote.cache_->pt_ = new policy_table::Table();

  // No application
  EXPECT_FALSE(access_remote.CheckModuleType("1234", policy_table::MT_RADIO));

  // No modules
  policy_table::ApplicationPolicies& apps =
      access_remote.cache_->pt_->policy_table.app_policies_section.apps;
  apps["1234"];
  EXPECT_FALSE(access_remote.CheckModuleType("1234", policy_table::MT_RADIO));

  // Empty modules
  policy_table::ModuleTypes& modules = *apps["1234"].moduleType;
  modules.mark_initialized();
  EXPECT_TRUE(access_remote.CheckModuleType("1234", policy_table::MT_RADIO));
  EXPECT_TRUE(access_remote.CheckModuleType("1234", policy_table::MT_CLIMATE));

  // Specific modules
  modules.push_back(policy_table::MT_RADIO);
  EXPECT_TRUE(access_remote.CheckModuleType("1234", policy_table::MT_RADIO));
  EXPECT_FALSE(access_remote.CheckModuleType("1234", policy_table::MT_CLIMATE));
}

TEST(AccessRemoteImplTest, EnableDisable) {
  AccessRemoteImpl access_remote;
  access_remote.cache_->pt_ = new policy_table::Table();

  // Country is enabled
  access_remote.enabled_ = true;
  access_remote.Enable();
  EXPECT_TRUE(access_remote.IsEnabled());

  access_remote.Disable();
  EXPECT_FALSE(access_remote.IsEnabled());

  // Country is disabled
  access_remote.enabled_ = false;
  access_remote.Enable();
  EXPECT_FALSE(access_remote.IsEnabled());

  access_remote.Disable();
  EXPECT_FALSE(access_remote.IsEnabled());
}

TEST(AccessRemoteImplTest, SetDefaultHmiTypes) {
  AccessRemoteImpl access_remote;

  std::vector<int> hmi_expected;
  hmi_expected.push_back(2);
  hmi_expected.push_back(6);
  Subject who = {"dev1", "1234"};
  access_remote.SetDefaultHmiTypes(who, hmi_expected);

  EXPECT_NE(access_remote.hmi_types_.end(), access_remote.hmi_types_.find(who));
  policy_table::AppHMITypes& hmi_output = access_remote.hmi_types_[who];
  EXPECT_EQ(2u, hmi_output.size());
  EXPECT_EQ(policy_table::AHT_MEDIA, hmi_output[0]);
  EXPECT_EQ(policy_table::AHT_SOCIAL, hmi_output[1]);
}

TEST(AccessRemoteImplTest, GetGroups) {
  AccessRemoteImpl access_remote;
  access_remote.enabled_ = true;
  Subject who = {"dev1", "1234"};
  access_remote.hmi_types_[who].push_back(policy_table::AHT_REMOTE_CONTROL);

  access_remote.cache_->pt_ = new policy_table::Table();
  policy_table::ApplicationPolicies& apps =
      access_remote.cache_->pt_->policy_table.app_policies_section.apps;
  apps["1234"].groups.push_back("group_default");
  apps["1234"].groups_primaryRC->push_back("group_primary");
  apps["1234"].AppHMIType->push_back(policy_table::AHT_MEDIA);

  // Default groups
  const policy_table::Strings& groups1 = access_remote.GetGroups(who);
  EXPECT_EQ(std::string("group_default"), std::string(groups1[0]));

  // Primary groups
  apps["1234"].set_to_string(policy::kDefaultId);
  const policy_table::Strings& groups2 = access_remote.GetGroups(who);
  EXPECT_EQ(std::string("group_primary"), std::string(groups2[0]));

  apps["1234"].set_to_string(policy::kDefaultId);
  Subject who2 = {"dev2", "1234"};

  // Empty groups
  access_remote.enabled_ = false;
  apps["1234"].set_to_string(policy::kDefaultId);
  const policy_table::Strings& groups4 = access_remote.GetGroups(who2);
  EXPECT_TRUE(groups4.empty());
}

}  // namespace policy
