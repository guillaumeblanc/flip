#pragma once

#include <map>

#include "sokol/sokol_app.h"

namespace flip {

// Keyboard object.
// Needs to be fed with application events
class Keyboard {
 public:
  explicit Keyboard(bool _repeat = false);

  // Forwards events to the Keyboard.
  bool Event(const sapp_event& _event);

  struct State {
    // True if key was released this frame.
    bool released() const { return released_frame == sapp_frame_count(); }

    // True if key is down.
    bool down = false;

    // Fame number at which key was released
    uint64_t released_frame = ~uint64_t{0};
  };

  struct Key {
    const char* name;
    State state;
  };

  using Keys = std::map<sapp_keycode, Key>;

  // Queries keycode's state
  State operator[](sapp_keycode _code) const {
    const auto& el = keys_.find(_code);
    if (el != keys_.end()) {
      return el->second.state;
    }
    return {};
  }

  // Access to all keycode's state, allows iterating
  const Keys& keys() const { return keys_; }

 private:
  Keys keys_;
  bool repeat_;
};

}  // namespace flip