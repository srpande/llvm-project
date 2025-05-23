//===-- ProtocolRequests.cpp ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "Protocol/ProtocolRequests.h"
#include "DAP.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/JSON.h"
#include <utility>

using namespace llvm;

namespace lldb_dap::protocol {

bool fromJSON(const llvm::json::Value &Params, CancelArguments &CA,
              llvm::json::Path P) {
  llvm::json::ObjectMapper O(Params, P);
  return O && O.map("requestId", CA.requestId) &&
         O.map("progressId", CA.progressId);
}

bool fromJSON(const json::Value &Params, DisconnectArguments &DA,
              json::Path P) {
  json::ObjectMapper O(Params, P);
  return O && O.map("restart", DA.restart) &&
         O.map("terminateDebuggee", DA.terminateDebuggee) &&
         O.map("suspendDebuggee", DA.suspendDebuggee);
}

bool fromJSON(const llvm::json::Value &Params, PathFormat &PF,
              llvm::json::Path P) {
  auto rawPathFormat = Params.getAsString();
  if (!rawPathFormat) {
    P.report("expected a string");
    return false;
  }

  std::optional<PathFormat> pathFormat =
      StringSwitch<std::optional<PathFormat>>(*rawPathFormat)
          .Case("path", ePatFormatPath)
          .Case("uri", ePathFormatURI)
          .Default(std::nullopt);
  if (!pathFormat) {
    P.report("unexpected value, expected 'path' or 'uri'");
    return false;
  }

  PF = *pathFormat;
  return true;
}

static const llvm::StringMap<ClientFeature> ClientFeatureByKey{
    {"supportsVariableType", eClientFeatureVariableType},
    {"supportsVariablePaging", eClientFeatureVariablePaging},
    {"supportsRunInTerminalRequest", eClientFeatureRunInTerminalRequest},
    {"supportsMemoryReferences", eClientFeatureMemoryReferences},
    {"supportsProgressReporting", eClientFeatureProgressReporting},
    {"supportsInvalidatedEvent", eClientFeatureInvalidatedEvent},
    {"supportsMemoryEvent", eClientFeatureMemoryEvent},
    {"supportsArgsCanBeInterpretedByShell",
     eClientFeatureArgsCanBeInterpretedByShell},
    {"supportsStartDebuggingRequest", eClientFeatureStartDebuggingRequest},
    {"supportsANSIStyling", eClientFeatureANSIStyling}};

bool fromJSON(const llvm::json::Value &Params, InitializeRequestArguments &IRA,
              llvm::json::Path P) {
  json::ObjectMapper OM(Params, P);
  if (!OM)
    return false;

  const json::Object *O = Params.getAsObject();

  for (auto &kv : ClientFeatureByKey) {
    const json::Value *value_ref = O->get(kv.first());
    if (!value_ref)
      continue;

    const std::optional<bool> value = value_ref->getAsBoolean();
    if (!value) {
      P.field(kv.first()).report("expected bool");
      return false;
    }

    if (*value)
      IRA.supportedFeatures.insert(kv.second);
  }

  return OM.map("adapterID", IRA.adapterID) &&
         OM.map("clientID", IRA.clientID) &&
         OM.map("clientName", IRA.clientName) && OM.map("locale", IRA.locale) &&
         OM.map("linesStartAt1", IRA.linesStartAt1) &&
         OM.map("columnsStartAt1", IRA.columnsStartAt1) &&
         OM.map("pathFormat", IRA.pathFormat) &&
         OM.map("$__lldb_sourceInitFile", IRA.lldbExtSourceInitFile);
}

bool fromJSON(const json::Value &Params, SourceArguments &SA, json::Path P) {
  json::ObjectMapper O(Params, P);
  return O && O.map("source", SA.source) &&
         O.map("sourceReference", SA.sourceReference);
}

json::Value toJSON(const SourceResponseBody &SA) {
  json::Object Result{{"content", SA.content}};

  if (SA.mimeType)
    Result.insert({"mimeType", SA.mimeType});

  return std::move(Result);
}

} // namespace lldb_dap::protocol
