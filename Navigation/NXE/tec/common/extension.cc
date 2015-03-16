// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extension.h"
#include "log.h"

#include <assert.h>
#include <vector>

namespace {

common::Extension* g_extension = NULL;
XW_Extension g_xw_extension = 0;

const XW_CoreInterface* g_core = NULL;
const XW_MessagingInterface* g_messaging = NULL;
const XW_Internal_SyncMessagingInterface* g_sync_messaging = NULL;
const XW_Internal_EntryPointsInterface* g_entry_points = NULL;
const XW_Internal_RuntimeInterface* g_runtime = NULL;
const XW_Internal_PermissionsInterface* g_permission = NULL;

bool InitializeInterfaces(XW_GetInterface get_interface) {
  g_core = reinterpret_cast<const XW_CoreInterface*>(
      get_interface(XW_CORE_INTERFACE));
  if (!g_core) {
    nError() << "Can't initialize extension: error getting Core interface";
    return false;
  }

  g_messaging = reinterpret_cast<const XW_MessagingInterface*>(
      get_interface(XW_MESSAGING_INTERFACE));
  if (!g_messaging) {
    nError() <<
        "Can't initialize extension: error getting Messaging interface.";
    return false;
  }

  g_sync_messaging =
      reinterpret_cast<const XW_Internal_SyncMessagingInterface*>(
          get_interface(XW_INTERNAL_SYNC_MESSAGING_INTERFACE));
  if (!g_sync_messaging) {
    nError() <<
        "Can't initialize extension: error getting SyncMessaging interface.";
    return false;
  }

  g_entry_points = reinterpret_cast<const XW_Internal_EntryPointsInterface*>(
      get_interface(XW_INTERNAL_ENTRY_POINTS_INTERFACE));
  if (!g_entry_points) {
    nError() << "NOTE: Entry points interface not available in this version "
              << "of Crosswalk, ignoring entry point data for extensions.";
  }

  g_runtime = reinterpret_cast<const XW_Internal_RuntimeInterface*>(
      get_interface(XW_INTERNAL_RUNTIME_INTERFACE));
  if (!g_runtime) {
    nError() << "NOTE: runtime interface not available in this version "
              << "of Crosswalk, ignoring runtime variables for extensions.";
  }

  g_permission = reinterpret_cast<const XW_Internal_PermissionsInterface*>(
      get_interface(XW_INTERNAL_PERMISSIONS_INTERFACE));
  if (!g_permission) {
    nError() << "NOTE: permission interface not available in this version "
      << "of Crosswalk, ignoring permission for extensions.";
  }

  return true;
}

}  // namespace

int32_t XW_Initialize(XW_Extension extension, XW_GetInterface get_interface) {
  assert(extension);
  g_xw_extension = extension;

  if (!InitializeInterfaces(get_interface))
    return XW_ERROR;

  g_extension = CreateExtension();
  //g_extension = nullptr;
  if (!g_extension) {
    nError() << "Can't initialize extension: "
              << "create extension returned NULL.";
    return XW_ERROR;
  }

  using common::Extension;
  g_core->RegisterShutdownCallback(g_xw_extension, Extension::OnShutdown);
  g_core->RegisterInstanceCallbacks(
      g_xw_extension, Extension::OnInstanceCreated,
      Extension::OnInstanceDestroyed);
  g_messaging->Register(g_xw_extension, Extension::HandleMessage);
  g_sync_messaging->Register(g_xw_extension, Extension::HandleSyncMessage);
  return XW_OK;
}

namespace common {

Extension::Extension() {}

Extension::~Extension() {}

void Extension::SetExtensionName(const char* name) {
  g_core->SetExtensionName(g_xw_extension, name);
}

void Extension::SetJavaScriptAPI(const char* api) {
  g_core->SetJavaScriptAPI(g_xw_extension, api);
}

void Extension::SetExtraJSEntryPoints(const char** entry_points) {
  if (g_entry_points)
    g_entry_points->SetExtraJSEntryPoints(g_xw_extension, entry_points);
}

bool Extension::RegisterPermissions(const char* perm_table) {
  if (g_permission)
    return g_permission->RegisterPermissions(g_xw_extension, perm_table);
  return false;
}

bool Extension::CheckAPIAccessControl(const char* api_name) {
  if (g_permission)
    return g_permission->CheckAPIAccessControl(g_xw_extension, api_name);
  return false;
}

Instance* Extension::CreateInstance() {
  return NULL;
}

std::string Extension::GetRuntimeVariable(const char* var_name, unsigned len) {
  if (!g_runtime)
    return "";

  std::vector<char> res(len + 1, 0);
  g_runtime->GetRuntimeVariableString(g_xw_extension, var_name, &res[0], len);
  return std::string(res.begin(), res.end());
}

// static
void Extension::OnShutdown(XW_Extension) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  delete g_extension;
  g_extension = NULL;
}

// static
void Extension::OnInstanceCreated(XW_Instance xw_instance) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  assert(!g_core->GetInstanceData(xw_instance));
  Instance* instance = g_extension->CreateInstance();
  if (!instance)
    return;
  instance->xw_instance_ = xw_instance;
  g_core->SetInstanceData(xw_instance, instance);
  instance->Initialize();
}

// static
void Extension::OnInstanceDestroyed(XW_Instance xw_instance) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  Instance* instance =
      reinterpret_cast<Instance*>(g_core->GetInstanceData(xw_instance));
  if (!instance)
    return;
  instance->xw_instance_ = 0;
  delete instance;
}

// static
void Extension::HandleMessage(XW_Instance xw_instance, const char* msg) {
  Instance* instance =
      reinterpret_cast<Instance*>(g_core->GetInstanceData(xw_instance));
  if (!instance)
    return;
  instance->HandleMessage(msg);
}

// static
void Extension::HandleSyncMessage(XW_Instance xw_instance, const char* msg) {
  Instance* instance =
      reinterpret_cast<Instance*>(g_core->GetInstanceData(xw_instance));
  if (!instance)
    return;
  instance->HandleSyncMessage(msg);
}

Instance::Instance()
    : xw_instance_(0) {}

Instance::~Instance() {
  assert(xw_instance_ == 0);
}

void Instance::PostMessage(const char* msg) {
  if (!xw_instance_) {
    nError() << "Ignoring PostMessage() in the constructor or after the "
              << "instance was destroyed.";
    return;
  }
  g_messaging->PostMessage(xw_instance_, msg);
}

void Instance::SendSyncReply(const char* reply) {
  if (!xw_instance_) {
    nError() << "Ignoring SendSyncReply() in the constructor or after the "
              << "instance was destroyed.";
    return;
  }
  g_sync_messaging->SetSyncReply(xw_instance_, reply);
}

}  // namespace common
