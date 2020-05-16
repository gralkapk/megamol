/*
 * Module.cpp
 *
 * Copyright (C) 2019 by Universitaet Stuttgart (VIS).
 * Alle Rechte vorbehalten.
 */

#include "stdafx.h"
#include "Module.h"

#include "Call.h"
#include "CallSlot.h"
#include "InterfaceSlot.h"


using namespace megamol;
using namespace megamol::gui;
using namespace megamol::gui::configurator;


megamol::gui::configurator::Module::Module(ImGuiID uid)
    : uid(uid)
    , class_name()
    , description()
    , plugin_name()
    , is_view(false)
    , parameters()
    , name()
    , is_view_instance(false)
    , callslots()
    , present() {

    this->callslots.emplace(megamol::gui::configurator::CallSlotType::CALLER, std::vector<CallSlotPtrType>());

    this->callslots.emplace(megamol::gui::configurator::CallSlotType::CALLEE, std::vector<CallSlotPtrType>());
}


megamol::gui::configurator::Module::~Module() {

    // Delete all call slots
    this->DeleteCallSlots();
}


bool megamol::gui::configurator::Module::AddCallSlot(megamol::gui::configurator::CallSlotPtrType callslot) {

    if (callslot == nullptr) {
        vislib::sys::Log::DefaultLog.WriteWarn(
            "Pointer to given call slot is nullptr. [%s, %s, line %d]\n", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    auto type = callslot->type;
    for (auto& callslot_ptr : this->callslots[type]) {
        if (callslot_ptr == callslot) {
            vislib::sys::Log::DefaultLog.WriteError(
                "Pointer to call slot already registered in modules call slot list. [%s, %s, line %d]\n", __FILE__,
                __FUNCTION__, __LINE__);
            return false;
        }
    }
    this->callslots[type].emplace_back(callslot);
    return true;
}


bool megamol::gui::configurator::Module::DeleteCallSlots(void) {

    try {
        for (auto& callslots_map : this->callslots) {
            for (auto callslot_iter = callslots_map.second.begin(); callslot_iter != callslots_map.second.end();
                 callslot_iter++) {
                (*callslot_iter)->DisconnectCalls();
                (*callslot_iter)->DisconnectParentModule();

                if ((*callslot_iter).use_count() > 1) {
                    vislib::sys::Log::DefaultLog.WriteError(
                        "Unclean deletion. Found %i references pointing to call slot. [%s, %s, line %d]\n",
                        (*callslot_iter).use_count(), __FILE__, __FUNCTION__, __LINE__);
                }

                (*callslot_iter).reset();
            }
            callslots_map.second.clear();
        }
    } catch (std::exception e) {
        vislib::sys::Log::DefaultLog.WriteError(
            "Error: %s [%s, %s, line %d]\n", e.what(), __FILE__, __FUNCTION__, __LINE__);
        return false;
    } catch (...) {
        vislib::sys::Log::DefaultLog.WriteError("Unknown Error. [%s, %s, line %d]\n", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    return true;
}


bool megamol::gui::configurator::Module::GetCallSlot(
    ImGuiID callslot_uid, megamol::gui::configurator::CallSlotPtrType& out_callslot_ptr) {

    if (callslot_uid != GUI_INVALID_ID) {
        for (auto& callslot_map : this->GetCallSlots()) {
            for (auto& callslot : callslot_map.second) {
                if (callslot->uid == callslot_uid) {
                    out_callslot_ptr = callslot;
                    return true;
                }
            }
        }
    }
    return false;
}


// MODULE PRESENTATION ####################################################

megamol::gui::configurator::Module::Presentation::Presentation(void)
    : group()
    , presentations(Module::Presentations::DEFAULT)
    , label_visible(true)
    , position(ImVec2(FLT_MAX, FLT_MAX))
    , size(ImVec2(0.0f, 0.0f))
    , utils()
    , selected(false)
    , update(true)
    , show_params(false)
    , set_screen_position(ImVec2(FLT_MAX, FLT_MAX))
    , set_selected_slot_position(false) {

    this->group.uid = GUI_INVALID_ID;
    this->group.visible = false;
    this->group.name = "";
}


megamol::gui::configurator::Module::Presentation::~Presentation(void) {}


void megamol::gui::configurator::Module::Presentation::Present(megamol::gui::PresentPhase phase,
    megamol::gui::configurator::Module& inout_module, megamol::gui::GraphItemsStateType& state) {

    if (ImGui::GetCurrentContext() == nullptr) {
        vislib::sys::Log::DefaultLog.WriteError(
            "No ImGui context available. [%s, %s, line %d]\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    ImGuiStyle& style = ImGui::GetStyle();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    assert(draw_list != nullptr);

    try {
        // Update size
        if (this->update || (this->size.x <= 0.0f) || (this->size.y <= 0.0f)) {
            this->Update(inout_module, state.canvas);
            this->update = false;
        }

        // Init position of newly created module (check after size update)
        if ((this->set_screen_position.x != FLT_MAX) && (this->set_screen_position.y != FLT_MAX)) {
            this->position = (this->set_screen_position - state.canvas.offset) / state.canvas.zooming;
            this->set_screen_position = ImVec2(FLT_MAX, FLT_MAX);
        }
        // Init position using current compatible slot
        if (this->set_selected_slot_position) {
            for (auto& callslot_map : inout_module.GetCallSlots()) {
                for (auto& callslot_ptr : callslot_map.second) {
                    CallSlotType callslot_type =
                        (callslot_ptr->type == CallSlotType::CALLEE) ? (CallSlotType::CALLER) : (CallSlotType::CALLEE);
                    for (auto& call_ptr : callslot_ptr->GetConnectedCalls()) {
                        auto connected_callslot_ptr = call_ptr->GetCallSlot(callslot_type);
                        float call_width =
                            (4.0f * GUI_GRAPH_BORDER + ImGui::CalcTextSize(call_ptr->class_name.c_str()).x);
                        if (state.interact.callslot_selected_uid != GUI_INVALID_ID) {
                            if ((connected_callslot_ptr->uid == state.interact.callslot_selected_uid) &&
                                connected_callslot_ptr->IsParentModuleConnected()) {
                                ImVec2 module_size = connected_callslot_ptr->GetParentModule()->GUI_GetSize();
                                ImVec2 module_pos = connected_callslot_ptr->GetParentModule()->GUI_GetPosition();
                                if (connected_callslot_ptr->type == CallSlotType::CALLEE) {
                                    this->position = module_pos - ImVec2((call_width + this->size.x), 0.0f);
                                } else {
                                    this->position = module_pos + ImVec2((call_width + module_size.x), 0.0f);
                                }
                                break;
                            }
                        } else if ((state.interact.interfaceslot_selected_uid != GUI_INVALID_ID) &&
                                   (connected_callslot_ptr->GUI_IsGroupInterface())) {
                            if (state.interact.interfaceslot_selected_uid ==
                                connected_callslot_ptr->GUI_GetGroupInterface()->uid) {
                                ImVec2 interfaceslot_position =
                                    (connected_callslot_ptr->GUI_GetGroupInterface()->GUI_GetPosition() -
                                        state.canvas.offset) /
                                    state.canvas.zooming;
                                if (connected_callslot_ptr->type == CallSlotType::CALLEE) {
                                    this->position = interfaceslot_position - ImVec2((call_width + this->size.x), 0.0f);
                                } else {
                                    this->position = interfaceslot_position + ImVec2(call_width, 0.0f);
                                }
                                break;
                            }
                        }
                    }
                }
            }
            this->set_selected_slot_position = false;
        }
        if ((this->position.x == FLT_MAX) && (this->position.y == FLT_MAX)) {
            // See layout border_offset in Graph::Presentation::layout_graph
            this->position = this->GetDefaultModulePosition(state.canvas);
        }

        // Check if module and call slots are visible
        bool visible =
            (this->group.uid == GUI_INVALID_ID) || ((this->group.uid != GUI_INVALID_ID) && this->group.visible);
        for (auto& callslots_map : inout_module.GetCallSlots()) {
            for (auto& callslot_ptr : callslots_map.second) {
                callslot_ptr->GUI_SetVisibility(visible);
            }
        }

        if (visible) {
            bool mouse_clicked_anywhere = ImGui::IsWindowHovered() && ImGui::GetIO().MouseClicked[0];

            ImGui::PushID(inout_module.uid);

            // Get current module information
            ImVec2 module_size = this->size * state.canvas.zooming;
            ImVec2 module_rect_min = state.canvas.offset + (this->position * state.canvas.zooming);
            ImVec2 module_rect_max = module_rect_min + module_size;
            ImVec2 module_center = module_rect_min + ImVec2(module_size.x / 2.0f, module_size.y / 2.0f);

            // Clip module if lying ouside the canvas
            /// Is there a benefit since ImGui::PushClipRect is used?
            ImVec2 canvas_rect_min = state.canvas.position;
            ImVec2 canvas_rect_max = state.canvas.position + state.canvas.size;
            if (!((canvas_rect_min.x < module_rect_max.x) && (canvas_rect_max.x > module_rect_min.x) &&
                    (canvas_rect_min.y < module_rect_max.y) && (canvas_rect_max.y > module_rect_min.y))) {
                if (mouse_clicked_anywhere) {
                    this->selected = false;
                    if (this->found_uid(state.interact.modules_selected_uids, inout_module.uid)) {
                        this->erase_uid(state.interact.modules_selected_uids, inout_module.uid);
                    }
                }
            } else {
                // MODULE ------------------------------------------------------
                std::string button_label = "module_" + std::to_string(inout_module.uid);

                if (phase == megamol::gui::PresentPhase::INTERACTION) {

                    // Button
                    ImGui::SetCursorScreenPos(module_rect_min);
                    ImGui::SetItemAllowOverlap();
                    ImGui::InvisibleButton(button_label.c_str(), module_size);
                    ImGui::SetItemAllowOverlap();
                    if (ImGui::IsItemActivated()) {
                        state.interact.button_active_uid = inout_module.uid;
                    }
                    if (ImGui::IsItemHovered()) {
                        state.interact.button_hovered_uid = inout_module.uid;
                    }

                    // Context menu
                    bool popup_rename = false;
                    if (ImGui::BeginPopupContextItem("invisible_button_context")) {
                        state.interact.button_active_uid = inout_module.uid;
                        bool singleselect = ((state.interact.modules_selected_uids.size() == 1) &&
                                             (this->found_uid(state.interact.modules_selected_uids, inout_module.uid)));

                        ImGui::TextUnformatted("Module");
                        ImGui::Separator();

                        if (ImGui::MenuItem(
                                "Delete", std::get<0>(state.hotkeys[megamol::gui::HotkeyIndex::DELETE_GRAPH_ITEM])
                                              .ToString()
                                              .c_str())) {
                            state.interact.process_deletion = true;
                        }
                        if (ImGui::MenuItem("Layout", nullptr, false, !singleselect)) {
                            state.interact.modules_layout = true;
                        }
                        if (ImGui::MenuItem("Rename", nullptr, false, singleselect)) {
                            popup_rename = true;
                        }
                        if (ImGui::BeginMenu("Add to Group", true)) {
                            if (ImGui::MenuItem("New")) {
                                state.interact.modules_add_group_uids.clear();
                                if (this->selected) {
                                    for (auto& module_uid : state.interact.modules_selected_uids) {
                                        state.interact.modules_add_group_uids.emplace_back(
                                            UIDPairType(module_uid, GUI_INVALID_ID));
                                    }
                                } else {
                                    state.interact.modules_add_group_uids.emplace_back(
                                        UIDPairType(inout_module.uid, GUI_INVALID_ID));
                                }
                            }
                            if (!state.groups.empty()) {
                                ImGui::Separator();
                            }
                            for (auto& group_pair : state.groups) {
                                if (ImGui::MenuItem(group_pair.second.c_str())) {
                                    state.interact.modules_add_group_uids.clear();
                                    if (this->selected) {
                                        for (auto& module_uid : state.interact.modules_selected_uids) {
                                            state.interact.modules_add_group_uids.emplace_back(
                                                UIDPairType(module_uid, group_pair.first));
                                        }
                                    } else {
                                        state.interact.modules_add_group_uids.emplace_back(
                                            UIDPairType(inout_module.uid, group_pair.first));
                                    }
                                }
                            }
                            ImGui::EndMenu();
                        }
                        if (ImGui::MenuItem("Remove from Group", nullptr, false, (this->group.uid != GUI_INVALID_ID))) {
                            state.interact.modules_remove_group_uids.clear();
                            if (this->selected) {
                                for (auto& module_uid : state.interact.modules_selected_uids) {
                                    state.interact.modules_remove_group_uids.emplace_back(module_uid);
                                }
                            } else {
                                state.interact.modules_remove_group_uids.emplace_back(inout_module.uid);
                            }
                        }

                        if (singleselect) {
                            ImGui::Separator();
                            ImGui::TextDisabled("Description");
                            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 13.0f);
                            ImGui::TextUnformatted(inout_module.description.c_str());
                            ImGui::PopTextWrapPos();
                        }
                        ImGui::EndPopup();
                    }

                    // Hover Tooltip
                    if ((state.interact.module_hovered_uid == inout_module.uid) && !this->label_visible) {
                        this->utils.HoverToolTip(inout_module.name, ImGui::GetID(button_label.c_str()), 0.5f, 5.0f);
                    } else {
                        this->utils.ResetHoverToolTip();
                    }

                    // Rename pop-up
                    if (this->utils.RenamePopUp("Rename Project", popup_rename, inout_module.name)) {
                        this->Update(inout_module, state.canvas);
                    }
                } else if (phase == megamol::gui::PresentPhase::RENDERING) {

                    bool active = (state.interact.button_active_uid == inout_module.uid);
                    bool hovered = (state.interact.button_hovered_uid == inout_module.uid);

                    // Selection
                    if (!this->selected &&
                        (active || this->found_uid(state.interact.modules_selected_uids, inout_module.uid))) {
                        if (!this->found_uid(state.interact.modules_selected_uids, inout_module.uid)) {
                            if (GUI_MULTISELECT_MODIFIER) {
                                // Multiple Selection
                                this->add_uid(state.interact.modules_selected_uids, inout_module.uid);
                            } else {
                                // Single Selection
                                state.interact.modules_selected_uids.clear();
                                state.interact.modules_selected_uids.emplace_back(inout_module.uid);
                            }
                        }
                        this->selected = true;
                        state.interact.callslot_selected_uid = GUI_INVALID_ID;
                        state.interact.call_selected_uid = GUI_INVALID_ID;
                        state.interact.group_selected_uid = GUI_INVALID_ID;
                        state.interact.interfaceslot_selected_uid = GUI_INVALID_ID;
                    }
                    // Deselection
                    else if (this->selected &&
                             ((mouse_clicked_anywhere && (state.interact.module_hovered_uid == GUI_INVALID_ID) &&
                                  !GUI_MULTISELECT_MODIFIER) ||
                                 (active && GUI_MULTISELECT_MODIFIER) ||
                                 (!this->found_uid(state.interact.modules_selected_uids, inout_module.uid)))) {
                        this->selected = false;
                        this->erase_uid(state.interact.modules_selected_uids, inout_module.uid);
                    }

                    // Dragging
                    if (this->selected && ImGui::IsWindowHovered() && ImGui::IsMouseDragging(0)) {
                        this->position += (ImGui::GetIO().MouseDelta / state.canvas.zooming);
                        this->Update(inout_module, state.canvas);
                    }

                    // Hovering
                    if (hovered) {
                        state.interact.module_hovered_uid = inout_module.uid;
                    }
                    if (!hovered && (state.interact.module_hovered_uid == inout_module.uid)) {
                        state.interact.module_hovered_uid = GUI_INVALID_ID;
                    }

                    // Colors
                    ImVec4 tmpcol = style.Colors[ImGuiCol_FrameBg];
                    tmpcol = ImVec4(tmpcol.x * tmpcol.w, tmpcol.y * tmpcol.w, tmpcol.z * tmpcol.w, 1.0f);
                    const ImU32 COLOR_MODULE_BACKGROUND = ImGui::ColorConvertFloat4ToU32(tmpcol);

                    tmpcol = style.Colors[ImGuiCol_FrameBgActive];
                    tmpcol = ImVec4(tmpcol.x * tmpcol.w, tmpcol.y * tmpcol.w, tmpcol.z * tmpcol.w, 1.0f);
                    const ImU32 COLOR_MODULE_HIGHTLIGHT = ImGui::ColorConvertFloat4ToU32(tmpcol);

                    tmpcol = style.Colors[ImGuiCol_ScrollbarGrabActive];
                    tmpcol = ImVec4(tmpcol.x * tmpcol.w, tmpcol.y * tmpcol.w, tmpcol.z * tmpcol.w, 1.0f);
                    const ImU32 COLOR_MODULE_BORDER = ImGui::ColorConvertFloat4ToU32(tmpcol);

                    const ImU32 COLOR_TEXT = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]);

                    const ImU32 COLOR_HEADER = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_FrameBgHovered]);

                    const ImU32 COLOR_HEADER_HIGHLIGHT =
                        ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_ButtonActive]);

                    // Draw Background
                    ImU32 module_bg_color = (this->selected) ? (COLOR_MODULE_HIGHTLIGHT) : (COLOR_MODULE_BACKGROUND);
                    draw_list->AddRectFilled(module_rect_min, module_rect_max, module_bg_color, GUI_RECT_CORNER_RADIUS,
                        ImDrawCornerFlags_All);

                    // Draw Text and Option Buttons
                    float text_width;
                    ImVec2 text_pos_left_upper;
                    const float line_height = ImGui::GetTextLineHeightWithSpacing();
                    ImVec2 param_child_pos;
                    bool other_item_hovered = false;

                    if (this->label_visible) {

                        bool main_view_button = inout_module.is_view;
                        bool parameter_button = (inout_module.parameters.size() > 0);
                        bool any_option_button = (main_view_button || parameter_button);

                        auto header_color = (this->selected) ? (COLOR_HEADER_HIGHLIGHT) : (COLOR_HEADER);
                        ImVec2 header_rect_max =
                            module_rect_min + ImVec2(module_size.x, ImGui::GetTextLineHeightWithSpacing());
                        draw_list->AddRectFilled(module_rect_min, header_rect_max, header_color, GUI_RECT_CORNER_RADIUS,
                            (ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight));

                        text_width = ImGui::CalcTextSize(inout_module.class_name.c_str()).x;
                        text_pos_left_upper = ImVec2(
                            module_center.x - (text_width / 2.0f), module_rect_min.y + (style.ItemSpacing.y / 2.0f));
                        draw_list->AddText(text_pos_left_upper, COLOR_TEXT, inout_module.class_name.c_str());

                        text_width = ImGui::CalcTextSize(inout_module.name.c_str()).x;
                        text_pos_left_upper =
                            module_center -
                            ImVec2((text_width / 2.0f), ((any_option_button) ? (line_height * 0.6f) : (0.0f)));
                        draw_list->AddText(text_pos_left_upper, COLOR_TEXT, inout_module.name.c_str());

                        if (any_option_button) {
                            float item_y_offset = (line_height / 2.0f);
                            float item_x_offset = (ImGui::GetFrameHeight() / 2.0f);
                            if (main_view_button && parameter_button) {
                                item_x_offset =
                                    ImGui::GetFrameHeight() + (0.5f * style.ItemSpacing.x * state.canvas.zooming);
                            }
                            ImGui::SetCursorScreenPos(module_center + ImVec2(-item_x_offset, item_y_offset));

                            if (main_view_button) {
                                if (ImGui::RadioButton("###main_view_switch", inout_module.is_view_instance)) {
                                    if (hovered) {
                                        state.interact.module_mainview_uid = inout_module.uid;
                                        inout_module.is_view_instance = !inout_module.is_view_instance;
                                    }
                                }
                                ImGui::SetItemAllowOverlap();
                                if (hovered) {
                                    other_item_hovered = other_item_hovered || this->utils.HoverToolTip("Main View");
                                }
                                ImGui::SameLine(0.0f, style.ItemSpacing.x * state.canvas.zooming);
                            }

                            if (parameter_button) {
                                param_child_pos = ImGui::GetCursorScreenPos();
                                param_child_pos.y += ImGui::GetFrameHeight();
                                if (ImGui::ArrowButton("###parameter_toggle",
                                        ((this->show_params) ? (ImGuiDir_Down) : (ImGuiDir_Up)))) {
                                    if (hovered) {
                                        this->show_params = !this->show_params;
                                    }
                                }
                                ImGui::SetItemAllowOverlap();
                                if (hovered) {
                                    other_item_hovered = other_item_hovered || this->utils.HoverToolTip("Parameters");
                                }
                            }
                        }
                    }

                    // Draw Outline
                    float border = ((inout_module.is_view_instance) ? (4.0f) : (1.0f)) * state.canvas.zooming;
                    draw_list->AddRect(module_rect_min, module_rect_max, COLOR_MODULE_BORDER, GUI_RECT_CORNER_RADIUS,
                        ImDrawCornerFlags_All, border);

                    // Parameter Child Window
                    if (this->label_visible && this->show_params) {
                        ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_MODULE_BACKGROUND);
                        ImGui::SetCursorScreenPos(param_child_pos);

                        float param_height = 0.0f;
                        for (auto& parameter : inout_module.parameters) {
                            param_height += parameter.GUI_GetHeight();
                        }
                        param_height += style.ScrollbarSize;
                        float avail_height =
                            (state.canvas.position.y + state.canvas.size.y) - ImGui::GetCursorScreenPos().y;
                        float child_height = std::min(avail_height, param_height);
                        float child_width = 325.0f * state.canvas.zooming;
                        auto child_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar |
                                           ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoMove |
                                           ImGuiWindowFlags_NavFlattened;
                        ImGui::BeginChild(
                            "module_parameter_child", ImVec2(child_width, child_height), true, child_flags);

                        // Close parameter window when clicked outside
                        // if (mouse_clicked_anywhere && !ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
                        //    this->show_params = false;
                        //}

                        for (auto& parameter : inout_module.parameters) {
                            parameter.GUI_Present();
                        }

                        ImGui::EndChild();
                        ImGui::PopStyleColor();
                        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
                            this->show_params = false;
                        }
                    }
                }
            }

            ImGui::PopID();

            // CALL SLOTS ------------------------------------------------------
            for (auto& callslots_map : inout_module.GetCallSlots()) {
                for (auto& callslot_ptr : callslots_map.second) {
                    callslot_ptr->GUI_Present(phase, state);
                }
            }
        }

    } catch (std::exception e) {
        vislib::sys::Log::DefaultLog.WriteError(
            "Error: %s [%s, %s, line %d]\n", e.what(), __FILE__, __FUNCTION__, __LINE__);
        return;
    } catch (...) {
        vislib::sys::Log::DefaultLog.WriteError("Unknown Error. [%s, %s, line %d]\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }
}


ImVec2 megamol::gui::configurator::Module::Presentation::GetDefaultModulePosition(const GraphCanvasType& canvas) {

    return ((ImVec2((2.0f * GUI_GRAPH_BORDER), (2.0f * GUI_GRAPH_BORDER)) + // ImGui::GetTextLineHeightWithSpacing()) +
                (canvas.position - canvas.offset)) /
            canvas.zooming);
}


void megamol::gui::configurator::Module::Presentation::Update(
    megamol::gui::configurator::Module& inout_module, const GraphCanvasType& in_canvas) {

    ImGuiStyle& style = ImGui::GetStyle();

    // WIDTH
    float class_width = 0.0f;
    float max_label_length = 0.0f;
    if (this->label_visible) {
        class_width = ImGui::CalcTextSize(inout_module.class_name.c_str()).x;
        float name_length = ImGui::CalcTextSize(inout_module.name.c_str()).x;
        float button_width =
            ((inout_module.is_view) ? (2.0f) : (1.0f)) * ImGui::GetTextLineHeightWithSpacing() + style.ItemSpacing.x;
        max_label_length = std::max(name_length, button_width);
    }
    max_label_length /= in_canvas.zooming;
    float max_slot_name_length = 0.0f;
    for (auto& callslots_map : inout_module.GetCallSlots()) {
        for (auto& callslot_ptr : callslots_map.second) {
            if (callslot_ptr->GUI_IsLabelVisible()) {
                max_slot_name_length =
                    std::max(ImGui::CalcTextSize(callslot_ptr->name.c_str()).x, max_slot_name_length);
            }
        }
    }
    if (max_slot_name_length > 0.0f) {
        max_slot_name_length = (2.0f * max_slot_name_length / in_canvas.zooming) + (1.0f * GUI_SLOT_RADIUS);
    }
    float module_width = std::max((class_width / in_canvas.zooming), (max_label_length + max_slot_name_length)) +
                         (3.0f * GUI_SLOT_RADIUS);

    // HEIGHT
    float line_height = (ImGui::GetTextLineHeightWithSpacing() / in_canvas.zooming);
    auto max_slot_count = std::max(
        inout_module.GetCallSlots(CallSlotType::CALLEE).size(), inout_module.GetCallSlots(CallSlotType::CALLER).size());
    float module_slot_height =
        line_height + (static_cast<float>(max_slot_count) * (GUI_SLOT_RADIUS * 2.0f) * 1.5f) + GUI_SLOT_RADIUS;
    float text_button_height = (line_height * ((this->label_visible) ? (4.0f) : (1.0f)));
    float module_height = std::max(module_slot_height, text_button_height);

    // Clamp to minimum size
    this->size = ImVec2(std::max(module_width, 100.0f), std::max(module_height, 50.0f));

    // UPDATE all Call Slots ---------------------
    for (auto& slot_pair : inout_module.GetCallSlots()) {
        for (auto& slot : slot_pair.second) {
            slot->GUI_Update(in_canvas);
        }
    }
}
