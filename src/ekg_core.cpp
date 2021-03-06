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
#include "ekg/api/ekg_core.hpp"
#include <list>

void ekg_core::process_event_section(SDL_Event &sdl_event) {
    // We do not need to track others events here.
    bool should_not_end_segment = (
            sdl_event.type == SDL_FINGERDOWN      || sdl_event.type == SDL_FINGERUP ||
            sdl_event.type == SDL_MOUSEBUTTONDOWN || sdl_event.type == SDL_MOUSEBUTTONUP ||
            sdl_event.type == SDL_MOUSEMOTION     || sdl_event.type == SDL_FINGERMOTION ||
            sdl_event.type == SDL_KEYDOWN         || sdl_event.type == SDL_KEYUP
    );

    // Process user event and resized window event.
    switch (sdl_event.type) {
        case SDL_USEREVENT: {
            if (sdl_event.user.code == EKG_EVENT) {
                should_not_end_segment = true;

                this->reset_current_event();
                this->current_poll_event = static_cast<ekg_event*>(sdl_event.user.data1);
            }

            break;
        }

        case SDL_WINDOWEVENT: {
            switch (sdl_event.window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    this->screen_width = static_cast<float>(sdl_event.window.data1);
                    this->screen_height = static_cast<float>(sdl_event.window.data2);
                    break;
                }
            }

            break;
        }
    }

    if (!should_not_end_segment) {
        return;
    }

    this->focused_element_id = 0;
    this->focused_element_type = 0;

    for (ekg_element* &element : this->data) {
        if (element == nullptr || element->access_flag().dead) {
            continue;
        }

        // Verify point overlap.
        element->on_pre_event_update(sdl_event);

        if (element->get_visibility() == ekg::visibility::VISIBLE && element->access_flag().over) {
            this->focused_element_id = element->get_id();
            this->focused_element_type = element->get_type();
        }

        element->on_post_event_update(sdl_event);
    }

    this->sizeof_render_buffer = 0;
    this->render_buffer.fill(0);
    this->data_update.clear();

    for (ekg_element* &element : this->data) {
        if (element == nullptr || element->access_flag().dead) {
            continue;
        }

        if (element->get_id() == this->focused_element_id) {
            element->on_pre_event_update(sdl_event);
        }

        element->on_event(sdl_event);

        if (element->get_visibility() == ekg::visibility::VISIBLE) {
            this->render_buffer[this->sizeof_render_buffer++] = element;
        }

        if (element->should_update()) {
            this->data_update.push_back(element);
        }
    }

    if (ekgapi::any_input_down(sdl_event) && this->focused_element_id != 0 && this->last_focused_element_id != this->focused_element_id) {
        this->last_focused_element_id = this->focused_element_id;
        this->fix_stack();
    }
}

void ekg_core::process_update_section() {
    if (ekgutil::contains(this->todo_flags, ekgutil::action::REFRESHUPDATE)) {
        ekgutil::remove(this->todo_flags, ekgutil::action::REFRESHUPDATE);
        this->swap_buffers();
    }

    for (ekg_element* &elements : this->data_update) {
        if (elements != nullptr) {
            elements->on_update();
        }
    }

    if (ekgutil::contains(this->todo_flags, ekgutil::action::SWAPBUFFERS)) {
        ekgutil::remove(this->todo_flags, ekgutil::action::SWAPBUFFERS);
        this->swap_buffers();
    }

    if (ekgutil::contains(this->todo_flags, ekgutil::action::FIXRECTS)) {
        ekgutil::remove(this->todo_flags, ekgutil::action::FIXRECTS);
        this->fix_rects();
    }

    if (ekgutil::contains(this->todo_flags, ekgutil::action::FIXSTACK)) {
        this->fix_stack();
    }
}

void ekg_core::process_render_section() {
    this->gpu_handler.prepare();

    if (ekgutil::contains(this->todo_flags, ekgutil::action::REFRESH)) {
        ekgutil::remove(this->todo_flags, ekgutil::action::REFRESH);
        ekggpu::invoke();

        // A fix to visual buffer problem that happens in the batching system.
        ekgfont::render("oi", -256, 0, ekg::theme().string_value_color);

        for (uint32_t i = 0; i < this->sizeof_render_buffer; i++) {
            ekg_element *&element = this->render_buffer[i];
            element->on_draw_refresh();
        }

        float x = 0;
        float y = 0;
        float w = 0;
        float h = 0;
        float offset_x = 0;
        float offset_y = 0;

        // Draw the immediate popups after draws the elements.
        // Clamp the sizes, draw first the background, string and at end the border.
        for (ekgutil::component &immediate_popup : this->immediate_popups) {
            w = ekgfont::get_text_width(immediate_popup.text);
            h = ekgfont::get_text_height(immediate_popup.text);

            offset_x = w / 10;
            offset_y = h / 10;

            x = immediate_popup.x - offset_x - (w / 2);
            y = immediate_popup.y - offset_y - h;

            ekgmath::clamp_aabb_with_screen_size(x, y, w + offset_x, h + offset_y);
            ekggpu::rectangle(x, y, w + offset_x, h + offset_y, this->theme_service.get_loaded_theme().immediate_popup_background);

            ekgfont::render(immediate_popup.text, x + (offset_x / 2), y + (offset_y / 2), this->theme_service.get_loaded_theme().string_value_color);
            ekggpu::rectangle(x, y, w + offset_x, h + offset_y, this->theme_service.get_loaded_theme().immediate_popup_border, 1);
        }

        // We do not want to render every time the immediate popups, so we clean after send to gpu.
        this->immediate_popups.clear();

        if (this->debug_mode) {
            ekgfont::render("Visual Background [" + std::to_string(this->sizeof_render_buffer) + ", " + std::to_string(this->data.size()) + "]", 10, 10, ekg::theme().string_value_color);
            ekgfont::render("Ticked buffers count: " + std::to_string(this->gpu_handler.get_ticked_refresh_buffers_count()), 10, 10 + ekgfont::get_text_height("oi"), ekg::theme().string_value_color);
        }

        ekggpu::revoke();
    }

    this->gpu_handler.calc_view_ortho_2d();
    this->gpu_handler.draw();
}

void ekg_core::add_element(ekg_element* element) {
    element->set_id(++this->last_id_used);
    this->concurrent_buffer.push_back(element);

    // Send tasks to the core.
    this->dispatch_todo_event(ekgutil::action::SWAPBUFFERS);
    this->dispatch_todo_event(ekgutil::action::FIXRECTS);
    this->force_reorder_stack(element->get_id());
}

void ekg_core::kill_element(ekg_element *element) {
    if (element->access_flag().dead) {
        return;
    }

    element->access_flag().dead = true;
    this->dispatch_todo_event(ekgutil::action::SWAPBUFFERS);
    this->dispatch_todo_event(ekgutil::action::REFRESH);
}

/* Start of refresh update. */
void ekg_core::refresh_update() {
    this->data_update.clear();

    for (ekg_element* &elements : this->data_update) {
        if (elements != nullptr && elements->should_update()) {
            this->data_update.push_back(elements);
        }
    }
}
/* End of refresh update. */

/* Start of swap buffers. */
void ekg_core::swap_buffers() {
    // Clean the buffer render (not delete).
    this->sizeof_render_buffer = 0;
    this->render_buffer.fill(nullptr);

    this->data_invisible_to_memory = this->data;
    this->data.clear();
    this->data_update.clear();

    for (ekg_element* &element : this->data_invisible_to_memory) {
        if (element == nullptr) {
            continue;
        }

        if (element->access_flag().dead) {
            delete element;
            element = nullptr;
            continue;
        }

        this->data.push_back(element);

        if (element->get_visibility() == ekg::visibility::VISIBLE) {
            this->render_buffer[this->sizeof_render_buffer++] = element;
        }

        if (element->should_update()) {
            this->data_update.push_back(element);
        }
    }

    for (ekg_element* &elements : this->concurrent_buffer) {
        if (elements == nullptr) {
            continue;
        }

        this->data.push_back(elements);

        if (elements->get_visibility() == ekg::visibility::VISIBLE) {
            this->render_buffer[this->sizeof_render_buffer++] = elements;
        }

        if (elements->should_update()) {
            this->data_update.push_back(elements);
        }
    }

    // Clean after push the buffers into main update buffer.
    this->concurrent_buffer.clear();
    this->data_invisible_to_memory.clear();
}
/* End of swap buffers. */

/* Start of fix stack. */
void ekg_core::fix_stack() {
    if (this->focused_element_id == 0 && this->forced_focused_element_id == 0) {
        return;
    }

    ekgutil::remove(this->todo_flags, ekgutil::action::FIXSTACK);

    if (this->focused_element_id == 0 || (this->forced_focused_element_id != 0 && this->focused_element_id != 0)) {
        this->focused_element_id = this->forced_focused_element_id;
    }

    ekgutil::stack concurrent_all_data_stack;
    ekgutil::stack concurrent_focused_stack;
    ekgutil::stack concurrent_data_stack;

    for (ekg_element* &elements : this->data) {
        if (elements == nullptr) {
            continue;
        }

        if (concurrent_all_data_stack.contains(elements->get_id()) || concurrent_focused_stack.contains(elements->get_id())) {
            continue;
        }

        concurrent_data_stack.ids.clear();
        elements->collect_stack(concurrent_data_stack);

        if (concurrent_data_stack.contains(this->focused_element_id)) {
            concurrent_focused_stack = concurrent_data_stack;
        } else {
            concurrent_all_data_stack.add(concurrent_data_stack);
        }
    }

    this->sizeof_render_buffer = 0;
    this->render_buffer.fill(nullptr);

    this->data_invisible_to_memory.clear();
    this->data_update.clear();

    ekg_element* element;

    for (uint32_t &ids : concurrent_all_data_stack.ids) {
        if (!this->find_element(element, ids)) {
            continue;
        }

        this->data_invisible_to_memory.push_back(element);

        if (element->get_visibility() == ekg::visibility::VISIBLE) {
            this->render_buffer[this->sizeof_render_buffer++] = element;
        }

        if (element->should_update()) {
            this->data_update.push_back(element);
        }
    }

    for (uint32_t &ids : concurrent_focused_stack.ids) {
        if (!this->find_element(element, ids)) {
            continue;
        }

        this->data_invisible_to_memory.push_back(element);

        if (element->get_visibility() == ekg::visibility::VISIBLE) {
            this->render_buffer[this->sizeof_render_buffer++] = element;
        }

        if (element->should_update()) {
            this->data_update.push_back(element);
        }
    }

    this->data = this->data_invisible_to_memory;
    this->data_invisible_to_memory.clear();
    this->forced_focused_element_id = 0;
}
/* End of fix stack. */

/* Start of fix rects. */
void ekg_core::fix_rects() {
    ekgutil::stack cached_stack;

    for (ekg_element* &element : this->data) {
        if (element == nullptr || element->access_flag().dead) {
            continue;
        }

        this->fix_rect(element, cached_stack);
    }
}

void ekg_core::fix_rect(ekg_element *&element, ekgutil::stack &cached_stack) {
    if (cached_stack.contains(element->get_id()) || !element->is_mother()) {
        return;
    }

    cached_stack.add(element->get_id());

    for (uint32_t &ids : element->access_children_stack().ids) {
        ekg_element* instance;

        if (cached_stack.contains(ids) || !this->find_element(instance, ids)) {
            continue;
        }

        instance->access_scaled_rect().x = element->get_x();
        instance->access_scaled_rect().y = element->get_y();
        instance->access_scaled_rect().w = element->get_width();
        instance->access_scaled_rect().h = element->get_height();
        instance->on_sync_position();

        cached_stack.add(ids);
        this->fix_rect(instance, cached_stack);
    }
}
/* End of fix rects. */

float ekg_core::get_screen_width() {
    return this->screen_width;
}

float ekg_core::get_screen_height() {
    return this->screen_height;
}

void ekg_core::set_instances(SDL_Window *&sdl_window) {
    this->sdl_window_instance = sdl_window;
}

void ekg_core::init() {
    this->gpu_handler.init();
    this->font_manager.init();
    this->theme_service.init();

    int32_t w, h;
    SDL_GetWindowSize(this->sdl_window_instance, &w, &h);

    this->screen_width = static_cast<float>(w);
    this->screen_height = static_cast<float>(h);
}

ekg_gpu_data_handler &ekg_core::get_gpu_handler() {
    return this->gpu_handler;
}

ekg_font &ekg_core::get_font_manager() {
    return this->font_manager;
}

void ekg_core::quit() {
    this->gpu_handler.quit();
    this->font_manager.quit();
}

void ekg_core::dispatch_todo_event(uint8_t flag) {
    ekgutil::add(this->todo_flags, flag);
}

bool ekg_core::find_element(ekg_element *&element, uint32_t id) {
    element = nullptr;

    for (ekg_element* &elements : this->concurrent_buffer) {
        if (elements != nullptr && elements->get_id() == id) {
            element = elements;
            return true;
        }
    }

    for (ekg_element* &elements : this->data) {
        if (elements != nullptr && elements->get_id() == id) {
            element = elements;
            return true;
        }
    }

    return false;
}

void ekg_core::force_reorder_stack(uint32_t id) {
    this->forced_focused_element_id = id;
    this->dispatch_todo_event(ekgutil::action::FIXSTACK);
}

ekg_theme_service &ekg_core::get_theme_service() {
    return this->theme_service;
}

void ekg_core::immediate_popup(float x, float y, const std::string &text) {
    ekgutil::component immediate_popup;

    immediate_popup.x = x;
    immediate_popup.y = y;
    immediate_popup.text = text;

    this->immediate_popups.push_back(immediate_popup);
}

uint32_t ekg_core::get_hovered_element_id() {
    return this->focused_element_id;
}

uint16_t ekg_core::get_hovered_element_type() {
    return this->focused_element_type;
}

uint32_t &ekg_core::get_popup_top_level() {
    return this->popup_top_level;
}

void ekg_core::reset_current_event() {
    if (this->current_poll_event != nullptr) {
        delete this->current_poll_event;
        this->current_poll_event = nullptr;

        if (this->debug_mode) {
            ekgutil::log("Deleted old event cache from memory.");
        }
    }
}

ekg_event* ekg_core::poll_event() {
    return this->current_poll_event;
}

void ekg_core::dispatch_event(SDL_Event &sdl_event) {
    sdl_event.type = SDL_USEREVENT;
    sdl_event.user.type = SDL_USEREVENT;
    SDL_PushEvent(&sdl_event);
}
