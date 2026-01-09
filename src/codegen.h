//
// Created by artur on 1/8/26.
//

#ifndef LOCUS_FX_CODEGEN_H
#define LOCUS_FX_CODEGEN_H

#include <string>
#include <symengine/basic.h>
namespace locusfx {
    std::string toWGSL(const SymEngine::RCP<const SymEngine::Basic> &expr);

    std::string generateShader(const std::string &equation);
}
#endif //LOCUS_FX_CODEGEN_H
