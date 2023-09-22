#include "flip/utils/profile.h"

#include <numeric>

#include "sokol/sokol_time.h"

namespace flip {

void ProfileRecord::push(float _value) {
  offset_ = ++offset_ % record_.size();
  record_[offset_] = _value;
}

ProfileStats stats(std::span<const float> _data) {
  return {*std::min_element(_data.begin(), _data.end()),
          std::accumulate(_data.begin(), _data.end(), 0.f) / _data.size(),
          *std::max_element(_data.begin(), _data.end())};
}

Profile::Profile(ProfileRecord& _record)
    : record_{_record}, start_{stm_now()} {}

Profile ::~Profile() {
  auto elapsed = stm_since(start_);
  record_.push(static_cast<float>(stm_ms(elapsed)));
}

}  // namespace flip