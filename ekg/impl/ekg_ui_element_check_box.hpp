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
#pragma once
#ifndef EKG_UI_ELEMENT_CHECK_BOX_H

#include "ekg_ui_element_abstract.hpp"

/**
 * # UI \n
 * # Author: Rina \n
 * # Since: 22/07/2022 | 03:30 am \n
 * # Type: Widget \n
 * # Description: Toggleable button. \n
 **/
class ekg_check_box : public ekg_element  {
protected:
    std::string text;

    float min_text_width;
    float min_text_height;

    float square_size = 0.0f;

    uint16_t enum_flags_text_dock;
    float text_offset_x = 0.0f, text_offset_y = 0.0f;
    float box_offset = 0.0f;
public:
    ekg_check_box();
    ~ekg_check_box();

    void set_text(const std::string &string);
    std::string get_text();

    void toggle();

    void set_checked(bool checked);
    bool is_checked();

    float get_min_text_width();
    float get_min_text_height();

    void set_text_dock(uint16_t flags);
    uint16_t get_text_dock();

    void set_width(float width);
    void set_height(float height);

    void set_size(float width, float height) override;
    void set_pos(float x, float y) override;
    void on_killed() override;
    void on_sync() override;
    void on_pre_event_update(SDL_Event &sdl_event) override;
    void on_event(SDL_Event &sdl_event) override;
    void on_post_event_update(SDL_Event &sdl_event) override;
    void on_draw_refresh() override;
};

#endif