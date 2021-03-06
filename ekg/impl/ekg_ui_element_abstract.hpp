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
#ifndef EKG_UI_ELEMENT_ABSTRACT_H
#define EKG_UI_ELEMENT_ABSTRACT_H

#include <SDL2/SDL.h>
#include <ekg/api/ekg_utility.hpp>

/**
 * # UI \n
 * # Type: Abstract \n
 * # Description: Invisible element, abstract \n
 **/
class ekg_element {
protected:
    bool update = false;
    uint16_t type = 0;
    int32_t scissor_id = -1;

    uint32_t id = 0;
    uint32_t visibility = 0;
    uint32_t mother_id = 0;

    ekgutil::flag flag;
    ekgutil::stack children_stack;

    ekgmath::rect scaled;
    ekgmath::rect cache;
    ekgmath::rect rect;

    float sync_x = 0;
    float sync_y = 0;

    std::string tag;
public:
    ekg_element();
    ~ekg_element();

    void set_tag(const std::string &str);
    std::string get_tag();

    void set_should_update(bool should_update);
    bool should_update();

    bool is_mother();
    bool has_mother();

    uint16_t get_type();

    void set_id(uint32_t element_id);
    uint32_t get_id();

    void set_visibility(uint32_t visible);
    uint32_t get_visibility();

    void set_mother_id(uint32_t element_id);
    uint32_t get_mother_id();

    ekgutil::flag &access_flag();
    ekgutil::stack &access_children_stack();
    ekgmath::rect &access_scaled_rect();

    float get_x();
    float get_y();

    float get_width();
    float get_height();

    float get_sync_x();
    float get_sync_y();

    void collect_stack(ekgutil::stack &stack);
    void on_sync_position();

    /* Start of abstract methods. */

    /*
     * Destroy/kill this element.
     */
    virtual void kill();

    /*
     * Set position of element.
     */
    virtual void set_pos(float x, float y);

    /*
     * Set size of element.
     */
    virtual void set_size(float width, float height);

    /*
     * Called when the element just die.
     */
    virtual void on_killed();

    /*
     * Sync data element stuff.
     */
    virtual void on_sync();

    /*
     * Update before process poll_event.
     */
    virtual void on_pre_event_update(SDL_Event &sdl_event);

    /*
     * Process poll_event.
     */
    virtual void on_event(SDL_Event &sdl_event);

    /*
     * Update after process poll_event.
     */
    virtual void on_post_event_update(SDL_Event &sdl_event);

    /*
     * Update stuff in element.
     */
    virtual void on_update();

    /*
     * "Draw" components present in element.
     */
    virtual void on_draw_refresh();

    /* End of abstract methods. */
};

#endif