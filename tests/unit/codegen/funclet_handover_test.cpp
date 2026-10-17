/**
 * @file        codegen/funclet_handover_test.cpp
 * @brief       Tests for the register hand-over around SEH funclet calls
 *
 * @copyright   Copyright (c) 2026 the ReXGlue SDK contributors (Ridge Racer 6
 *              recompilation fork)
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */

#include <catch2/catch_test_macros.hpp>

#include <string>

#include "codegen/builders/builder_context.h"

using namespace rex::codegen;

namespace {

std::string Body(const char* indent) {
  return std::string(indent) + std::string(kFuncletHandOverMarker) + "\n" + indent +
         "sub_8224BD0C(ctx, base);\n" + indent + std::string(kFuncletTakeBackMarker) + "\n";
}

}  // namespace

TEST_CASE("Funclet markers vanish when nothing is localized", "[codegen][funclet]") {
  RecompilerLocalVariables locals;
  CHECK(ExpandFuncletMarkers(Body("\t"), locals) == "\tsub_8224BD0C(ctx, base);\n");
}

TEST_CASE("Funclet markers copy every localized register both ways", "[codegen][funclet]") {
  RecompilerLocalVariables locals;
  locals.r[12] = true;
  locals.r[31] = true;
  locals.f[0] = true;
  locals.v[40] = true;
  locals.cr[6] = true;
  locals.ctr = true;
  locals.xer = true;
  locals.reserved = true;
  locals.temp = true;  // scratch, not a register: never handed over

  const std::string expected =
      "\tctx.r12 = r12;\n\tctx.r31 = r31;\n\tctx.f0 = f0;\n\tctx.v40 = v40;\n"
      "\tctx.cr6 = cr6;\n\tctx.ctr = ctr;\n\tctx.xer = xer;\n\tctx.reserved = reserved;\n"
      "\tsub_8224BD0C(ctx, base);\n"
      "\tr12 = ctx.r12;\n\tr31 = ctx.r31;\n\tf0 = ctx.f0;\n\tv40 = ctx.v40;\n"
      "\tcr6 = ctx.cr6;\n\tctr = ctx.ctr;\n\txer = ctx.xer;\n\treserved = ctx.reserved;\n";
  CHECK(ExpandFuncletMarkers(Body("\t"), locals) == expected);
}

TEST_CASE("Funclet markers keep the marker line's indentation", "[codegen][funclet]") {
  RecompilerLocalVariables locals;
  locals.r[11] = true;
  CHECK(ExpandFuncletMarkers(Body("\t\t\t"), locals) ==
        "\t\t\tctx.r11 = r11;\n\t\t\tsub_8224BD0C(ctx, base);\n\t\t\tr11 = ctx.r11;\n");
}

TEST_CASE("Bodies without funclet calls are returned unchanged", "[codegen][funclet]") {
  RecompilerLocalVariables locals;
  locals.r[12] = true;
  const std::string body = "\tr12.u64 = ctx.lr;\n\t// a comment\n\tsub_82000000(ctx, base);";
  CHECK(ExpandFuncletMarkers(body, locals) == body);
}
