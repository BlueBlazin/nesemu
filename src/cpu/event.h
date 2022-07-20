#ifndef SRC_CPU_EVENT_H_
#define SRC_CPU_EVENT_H_

namespace cpu {

enum class Event {
  VBlank,
  MaxCycles,
  Stopped,
};

}  // namespace cpu

#endif  // SRC_CPU_EVENT_H_