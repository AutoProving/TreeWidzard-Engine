#include "Kernel/Bag.h"
#include "Kernel/CoreWrapper.h"
#include "Kernel/WitnessSet.h"
#include "Kernel/WitnessWrapper.h"

#define CONCAT(L, R) L##_##R
#define CONCAT_EXPANDED(L, R) CONCAT(L, R)

#define Witness CONCAT_EXPANDED(CORE_FILE_NAME, Witness)
#define Core CONCAT_EXPANDED(CORE_FILE_NAME, Core)

struct Witness;
struct Core;

using WitnessPointer = std::shared_ptr<Witness>;
using WitnessPointerConst = std::shared_ptr<const Witness>;

template <class T, class... Args>
DynamicCore *create_and_cast_to_dyn_core(Args &&...args) {
  return new T(std::forward<Args>(args)...);
}

extern "C" {
DynamicCore *create() { return create_and_cast_to_dyn_core<Core>(); }
//DynamicCore *create_int(int x) { return create_and_cast_to_dyn_core<Core>(x); }
//DynamicCore *create_multiGraph(MultiGraph g) { return create_and_cast_to_dyn_core<Core>(std::move(g)); }
//DynamicCore *create_parameters(vector<int> a) { return create_and_cast_to_dyn_core<Core>(std::move(a)); }
}
