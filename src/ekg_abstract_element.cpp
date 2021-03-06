/**
 * EKG-LICENSE - this software/library LICENSE can not be modified in any instance.
 *
 * --
 * ANY NON-OFFICIAL MODIFICATION IS A CRIME.
 * DO NOT SELL THIS CODE SOFTWARE, FOR USE EKG IN A COMMERCIAL PRODUCT ADD EKG-LICENSE TO PROJECT,
 * RESPECT THE COPYRIGHT TERMS OF EKG, NO SELL WITHOUT EKG-LICENSE (IT IS A CRIME).
 * DO NOT FORK THE PROJECT SOURCE WITHOUT EKG-LICENSE.
 *
 * END OF EKG-LICENSE.
 **/
#include <ekg/ekg.hpp>
#include "ekg/impl/ekg_ui_element_abstract.hpp"


ekg_element::ekg_element() {
    this->type = ekg::ui::ABSTRACT;
}

ekg_element::~ekg_element() {

}

void ekg_element::set_should_update(bool should_update) {
    // Prevent loop.
    if (this->update != should_update && should_update) {
        the_ekg_core->dispatch_todo_event(ekgutil::action::REFRESHUPDATE);
    }

    this->update = should_update;
}

bool ekg_element::should_update() {
    return this->update;
}

void ekg_element::on_pre_event_update(SDL_Event &sdl_event) {

}

void ekg_element::on_event(SDL_Event &sdl_event) {

}

void ekg_element::on_post_event_update(SDL_Event &sdl_event) {

}

void ekg_element::on_update() {

}

void ekg_element::on_draw_refresh() {

}

void ekg_element::on_killed() {

}

void ekg_element::on_sync() {

}

void ekg_element::set_id(uint32_t element_id) {
    this->id = element_id;
}

uint16_t ekg_element::get_type() {
    return this->type;
}

void ekg_element::set_visibility(uint32_t visible) {
    this->visibility = visible;
}

uint32_t ekg_element::get_visibility() {
    return this->visibility;
}

void ekg_element::set_mother_id(uint32_t element_id) {
    if (this->mother_id != element_id) {
        this->mother_id = element_id;

        if (element_id == 0) {
            this->scaled.x = 0;
            this->scaled.y = 0;
            this->scaled.w = 0;
            this->scaled.h = 0;
            return;
        }

        the_ekg_core->dispatch_todo_event(ekgutil::action::FIXRECTS);
    }
}

uint32_t ekg_element::get_id() {
    return this->id;
}

ekgutil::flag &ekg_element::access_flag() {
    return this->flag;
}

ekgutil::stack &ekg_element::access_children_stack() {
    return this->children_stack;
}

ekgmath::rect &ekg_element::access_scaled_rect() {
    return this->scaled;
}

float ekg_element::get_x() {
    return this->rect.x;
}

float ekg_element::get_y() {
    return this->rect.y;
}

float ekg_element::get_width() {
    return this->rect.w;
}

float ekg_element::get_height() {
    return this->rect.h;
}

bool ekg_element::is_mother() {
    return !this->children_stack.ids.empty();
}

bool ekg_element::has_mother() {
    return this->mother_id != 0;
}

void ekg_element::set_pos(float x, float y) {
    x += this->scaled.x;
    y += this->scaled.y;

    if (this->has_mother() && (this->rect.x != x || this->rect.y != y)) {
        x -= this->scaled.x;
        y -= this->scaled.y;

        this->sync_x = x;
        this->sync_y = y;

        this->on_sync_position();
        this->on_sync();
    } else if (!this->has_mother() && (this->rect.x != x || this->rect.y != y)) {
        this->rect.x = x;
        this->rect.y = y;

        this->on_sync_position();
        this->on_sync();
    }
}

void ekg_element::set_size(float width, float height) {;
}

void ekg_element::on_sync_position() {
    if (this->has_mother()) {
        this->rect.x = this->scaled.x + this->sync_x;
        this->rect.y = this->scaled.y + this->sync_y;
    }
}

float ekg_element::get_sync_x() {
    return this->sync_x;
}

float ekg_element::get_sync_y() {
    return  this->sync_y;
}

uint32_t ekg_element::get_mother_id() {
    return this->mother_id;
}

void ekg_element::collect_stack(ekgutil::stack &stack) {
    if (stack.contains(this->id)) {
        return;
    }

    stack.add(this->id);
    ekg_element* element;

    for (uint32_t &ids : this->children_stack.ids) {
        if (the_ekg_core->find_element(element, ids)) {
            element->collect_stack(stack);
        }
    }
}

void ekg_element::kill() {
    the_ekg_core->kill_element(this);
    this->set_visibility(ekg::visibility::DISABLED);
}

void ekg_element::set_tag(const std::string &str) {

}

std::string ekg_element::get_tag() {
    return std::string();
}
