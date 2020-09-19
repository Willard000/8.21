#include "GUIFunctions.h"

#include "../src/System/Environment.h"
#include "../src/System/InputManager.h"

void bt_set_mode_place_entity() {
	static_cast<EditorInputManager*>(Environment::get().get_input_manager())->set_mode(EDITOR_PLACE_ENTITY);
}

void bt_set_mode_select_entity() {
	static_cast<EditorInputManager*>(Environment::get().get_input_manager())->set_mode(EDITOR_SELECT_ENTITY);
}

void bt_set_mode_edit_terrain() {
	static_cast<EditorInputManager*>(Environment::get().get_input_manager())->set_mode(EDITOR_EDIT_TERRAIN);
}