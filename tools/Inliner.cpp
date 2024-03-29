#include "OpenQASMCompiler/Support/CommandLine.h"
#include "OpenQASMCompiler/Transform/QModule.h"
#include "OpenQASMCompiler/Transform/Driver.h"
#include "OpenQASMCompiler/Transform/PassCache.h"
#include "OpenQASMCompiler/Transform/QModuleQualityEvalPass.h"
#include "OpenQASMCompiler/Transform/InlineAllPass.h"
#include "OpenQASMCompiler/Transform/ReverseEdgesPass.h"
#include "OpenQASMCompiler/Transform/FlattenPass.h"
#include "OpenQASMCompiler/Transform/Allocators/Allocators.h"
#include "OpenQASMCompiler/Transform/Utils.h"
#include "OpenQASMCompiler/Arch/Architectures.h"
#include "OpenQASMCompiler/Support/JsonParser.h"
#include "OpenQASMCompiler/Support/Stats.h"
#include "OpenQASMCompiler/Support/Defs.h"

#include <fstream>
#include <cassert>
#include <cctype>
#include <sstream>
#include <map>

using namespace efd;

namespace efd {
    template <> std::string Opt<GateWeightMap>::getStringVal() {
        std::string str;
        for (auto& pair : mVal)
            str += pair.first + ":" + std::to_string(pair.second) + " ";
        return str;
    }

    template <> struct ParseOptTrait<GateWeightMap> {
        static void Run(Opt<GateWeightMap>* opt, std::vector<std::string> args) {
            std::string sVector = args[0];
            std::istringstream iVector(sVector);

            for (std::string str; iVector >> str;) {
                std::size_t i = str.find(':');
                int32_t l = i, r = i;

                if (i != std::string::npos) {
                    while (l >= 0 && !isspace(str[l])) --l;
                    while (r < (int32_t) str.length() && !isspace(str[r])) ++r;
                    ++l; --r;
                }

                std::string key = str.substr(l, i - l);
                std::string val = str.substr(i + 1, r - i);
                opt->mVal[key] = std::stoull(val);
            }
        }
    };
}

static efd::Opt<GateWeightMap> GateWeights
("-gate-w",
"Cost of using each basis gate. \
Should be specified as <gate>:<w> between quotes.",
{{"U", 1}, {"CX", 10}}, false);

static Opt<std::string> InFilepath
("i", "The input file.", "", true);
static Opt<std::string> OutFilepath
("o", "The output file.", "", false);

static Opt<bool> NoPretty
("-no-pretty", "Print in a pretty format (negation).", false, false);

static void DumpToOutFile(QModule::Ref qmod) {
    std::ofstream cmdOut(OutFilepath.getVal());
    std::ostream& out = (OutFilepath.getVal() != "") ? cmdOut : std::cout;
    PrintToStream(qmod, out, !NoPretty.getVal());
    cmdOut.close();
}

int main(int argc, char** argv) {
    Init(argc, argv);
    QModule::uRef qmod = ParseFile(InFilepath.getVal());

    if (qmod.get() != nullptr) {
        auto inlinePass = InlineAllPass::Create(ExtractGateNames(GateWeights.getVal()));
        PassCache::Run(qmod.get(), inlinePass.get());
        PassCache::Run<FlattenPass>(qmod.get());
        DumpToOutFile(qmod.get());
    }

    return 0;
}
