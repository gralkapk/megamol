/**
 * MegaMol
 * Copyright (c) 2026, MegaMol Dev Team
 * All rights reserved.
 */

#include "mmcore/factories/AbstractPluginInstance.h"
#include "mmcore/factories/PluginRegister.h"

#include "ArrowDataSource.h"
#include "ArrowToTableData.h"

#include "mmarrow/ArrowDataCall.h"

namespace megamol::mmarrow {
class PluginInstance : public megamol::core::factories::AbstractPluginInstance {
    REGISTERPLUGIN(PluginInstance)
public:
    PluginInstance()
            : megamol::core::factories::AbstractPluginInstance(
                  // machine-readable plugin assembly name
                  "mmarrow", // TODO: Change this!

                  // human-readable plugin description
                  "Plugin for arrow-based IO") {};

    ~PluginInstance() override = default;

    // Registers modules and calls
    void registerClasses() override {

        // register modules
        this->module_descriptions.RegisterAutoDescription<ArrowDataSource>();
        this->module_descriptions.RegisterAutoDescription<ArrowToTableData>();
        //
        // TODO: Register your plugin's modules here:
        // this->module_descriptions.RegisterAutoDescription<megamol::MegaMolPlugin::MyModule1>();
        // this->module_descriptions.RegisterAutoDescription<megamol::MegaMolPlugin::MyModule2>();
        // ...
        //

        // register calls
        this->call_descriptions.RegisterAutoDescription<ArrowDataCall>();
        // TODO: Register your plugin's calls here:
        // this->call_descriptions.RegisterAutoDescription<megamol::MegaMolPlugin::MyCall1>();
        // this->call_descriptions.RegisterAutoDescription<megamol::MegaMolPlugin::MyCall2>();
        // ...
        //
    }
};
} // namespace megamol::mmarrow
