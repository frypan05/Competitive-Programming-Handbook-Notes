#include <map>
template<typename K, typename V>
class interval_map{
  friend void IntervalMapTest();
  V m_valBegin;
  std::map<k,V> m_map;

public:
  template<typename V_forward>
  interval_map(V_forward&& val)
  : m_valBegin(std:: forward<V_forward>(val))
  {}


  template<typename V_forward>
  void assign (K const& keyBegin, K const& keyEnd, V_forward&& val)

  requires (std)
}
