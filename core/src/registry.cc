#include "promcore/registry.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <tuple>

#include "promcore/counter.h"
#include "promcore/detail/future_std.h"
#include "promcore/gauge.h"
#include "promcore/histogram.h"
#include "promcore/info.h"
#include "promcore/summary.h"

namespace promcore {

namespace {
template <typename T>
void CollectAll(std::vector<MetricFamily>& results, const T& families) {
  for (auto&& collectable : families) {
    auto metrics = collectable->Collect();
    results.insert(results.end(), std::make_move_iterator(metrics.begin()),
                   std::make_move_iterator(metrics.end()));
  }
}

bool FamilyNameExists(const std::string& /* name */) { return false; }

template <typename T, typename... Args>
bool FamilyNameExists(const std::string& name, const T& families,
                      Args&&... args) {
  auto sameName = [&name](const typename T::value_type& entry) {
    return name == entry->GetName();
  };
  auto exists = std::find_if(std::begin(families), std::end(families),
                             sameName) != std::end(families);
  return exists || FamilyNameExists(name, args...);
}
}  // namespace

Registry::Registry(InsertBehavior insert_behavior)
    : insert_behavior_{insert_behavior} {}

Registry::~Registry() = default;

std::vector<MetricFamily> Registry::Collect() const {
  std::lock_guard<std::mutex> lock{mutex_};
  auto results = std::vector<MetricFamily>{};

  CollectAll(results, countersDouble_);
  CollectAll(results, countersInt_);
  CollectAll(results, gaugesDouble_);
  CollectAll(results, gaugesInt_);
  CollectAll(results, histograms_);
  CollectAll(results, infos_);
  CollectAll(results, summaries_);

  return results;
}

template <>
std::vector<std::unique_ptr<Family<CounterDouble>>>& Registry::GetFamilies() {
  return countersDouble_;
}

template <>
std::vector<std::unique_ptr<Family<CounterInt>>>& Registry::GetFamilies() {
  return countersInt_;
}

template <>
std::vector<std::unique_ptr<Family<GaugeDouble>>>& Registry::GetFamilies() {
  return gaugesDouble_;
}

template <>
std::vector<std::unique_ptr<Family<GaugeInt>>>& Registry::GetFamilies() {
  return gaugesInt_;
}

template <>
std::vector<std::unique_ptr<Family<Histogram>>>& Registry::GetFamilies() {
  return histograms_;
}

template <>
std::vector<std::unique_ptr<Family<Info>>>& Registry::GetFamilies() {
  return infos_;
}

template <>
std::vector<std::unique_ptr<Family<Summary>>>& Registry::GetFamilies() {
  return summaries_;
}

template <>
bool Registry::NameExistsInOtherType<CounterDouble>(const std::string& name) const {
  return FamilyNameExists(name, countersInt_, gaugesDouble_, gaugesInt_, histograms_, infos_, summaries_);
}

template <>
bool Registry::NameExistsInOtherType<CounterInt>(const std::string& name) const {
  return FamilyNameExists(name, countersDouble_, gaugesDouble_, gaugesInt_, histograms_, infos_, summaries_);
}

template <>
bool Registry::NameExistsInOtherType<GaugeDouble>(const std::string& name) const {
  return FamilyNameExists(name, countersDouble_, countersInt_, gaugesInt_, histograms_, infos_, summaries_);
}

template <>
bool Registry::NameExistsInOtherType<GaugeInt>(const std::string& name) const {
  return FamilyNameExists(name, countersDouble_, countersInt_, gaugesDouble_, histograms_, infos_, summaries_);
}

template <>
bool Registry::NameExistsInOtherType<Histogram>(const std::string& name) const {
  return FamilyNameExists(name, countersDouble_, countersInt_, gaugesDouble_, gaugesInt_, infos_, summaries_);
}

template <>
bool Registry::NameExistsInOtherType<Info>(const std::string& name) const {
  return FamilyNameExists(name, countersDouble_, countersInt_, gaugesDouble_, gaugesInt_, histograms_, summaries_);
}

template <>
bool Registry::NameExistsInOtherType<Summary>(const std::string& name) const {
  return FamilyNameExists(name, countersDouble_, countersInt_, gaugesDouble_, gaugesInt_, histograms_, infos_);
}

template <typename T>
Family<T>& Registry::Add(const std::string& name, const std::string& help,
                         const Labels& labels) {
  std::lock_guard<std::mutex> lock{mutex_};

  if (NameExistsInOtherType<T>(name)) {
    throw std::invalid_argument(
        "Family name already exists with different type");
  }

  auto& families = GetFamilies<T>();

  auto same_name = [&name](const std::unique_ptr<Family<T>>& family) {
    return name == family->GetName();
  };

  auto it = std::find_if(families.begin(), families.end(), same_name);
  if (it != families.end()) {
    if (insert_behavior_ == InsertBehavior::Merge) {
      if ((*it)->GetConstantLabels() == labels) {
        return **it;
      }
      throw std::invalid_argument(
          "Family name already exists with different constant labels");
    } else {
      throw std::invalid_argument("Family name already exists");
    }
  }

  auto family = detail::make_unique<Family<T>>(name, help, labels);
  auto& ref = *family;
  families.push_back(std::move(family));
  return ref;
}

template Family<CounterDouble>& Registry::Add(const std::string& name,
                                        const std::string& help,
                                        const Labels& labels);

template Family<CounterInt>& Registry::Add(const std::string& name,
                                        const std::string& help,
                                        const Labels& labels);

template Family<GaugeDouble>& Registry::Add(const std::string& name,
                                      const std::string& help,
                                      const Labels& labels);

template Family<GaugeInt>& Registry::Add(const std::string& name,
                                      const std::string& help,
                                      const Labels& labels);

template Family<Info>& Registry::Add(const std::string& name,
                                     const std::string& help,
                                     const Labels& labels);

template Family<Summary>& Registry::Add(const std::string& name,
                                        const std::string& help,
                                        const Labels& labels);

template Family<Histogram>& Registry::Add(const std::string& name,
                                          const std::string& help,
                                          const Labels& labels);

template <typename T>
bool Registry::Remove(const Family<T>& family) {
  std::lock_guard<std::mutex> lock{mutex_};

  auto& families = GetFamilies<T>();
  auto same_family = [&family](const std::unique_ptr<Family<T>>& in) {
    return &family == in.get();
  };

  auto it = std::find_if(families.begin(), families.end(), same_family);
  if (it == families.end()) {
    return false;
  }

  families.erase(it);
  return true;
}

template bool PROMCORE_CPP_CORE_EXPORT
Registry::Remove(const Family<CounterDouble>& family);

template bool PROMCORE_CPP_CORE_EXPORT
Registry::Remove(const Family<CounterInt>& family);

template bool PROMCORE_CPP_CORE_EXPORT
Registry::Remove(const Family<GaugeDouble>& family);

template bool PROMCORE_CPP_CORE_EXPORT
Registry::Remove(const Family<GaugeInt>& family);

template bool PROMCORE_CPP_CORE_EXPORT
Registry::Remove(const Family<Summary>& family);

template bool PROMCORE_CPP_CORE_EXPORT
Registry::Remove(const Family<Histogram>& family);

template bool PROMCORE_CPP_CORE_EXPORT
Registry::Remove(const Family<Info>& family);

}  // namespace promcore
