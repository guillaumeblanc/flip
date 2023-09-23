#pragma once
#include <array>
#include <cstdint>
#include <span>

namespace flip {

class ProfileRecord {
 public:
  void push(float _value);

  float front() const { return record_[offset_]; }

  struct View {
    std::span<const float> data;
    int offset;
  };
  View view() const { return {{record_}, offset_}; }

 private:
  std::array<float, 64> record_ = {0};
  int offset_ = 0;
};

struct ProfileStats {
  float min;
  float mean;
  float max;
};
ProfileStats stats(std::span<const float> _data);

class Profile {
 public:
  Profile(ProfileRecord& _record);
  ~Profile();

 private:
  ProfileRecord& record_;
  uint64_t start_;
};
}  // namespace flip