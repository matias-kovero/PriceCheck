#include "pch.h"
#include "APIClasses.h"

#define J(var) j.at(#var).get_to(p.var);
#define J2(var, var2) j.at(#var).get_to(p.var2);

#define JOPT(var) if(j.find(#var) != j.end()) { j.at(#var).get_to(p.var); }
#define JOPT2(var, var2) if(j.find(#var) != j.end()) { j.at(#var).get_to(p.var2); }

void from_json(const json j, PaintPrice& p)
{
  JOPT(min);
  JOPT(max);
}

void from_json(const json j, Item& p)
{
  JOPT(id);
  JOPT2(error, isError);
  JOPT2(time, last_refresh);
  J(data);
}

void from_json(const json j, Blueprint& p)
{
  JOPT(id);
  JOPT2(error, isError);
  JOPT2(time, last_refresh);
  J(data);
}

void from_json(const json j, APIData& p)
{
  J2(error, isError);
  J(count);
  J2(time, last_refresh);
  J(items);
  J(prints);

  for (auto& [id, item] : p.items)
  {
    item.id = id;
    item.isError = p.isError;
    item.last_refresh = p.last_refresh;
  }

  for (auto& [id, bp] : p.prints)
  {
    bp.id = id;
    bp.isError = p.isError;
    bp.last_refresh = p.last_refresh;
  }
}

void from_json(const json j, APIResponse& p)
{
  J(count);
  J2(time, last_refresh);
  J2(error, isError);
  J(data);

  for (auto& [id, item] : p.data)
  {
    item.id = id;
    item.isError = p.isError;
    item.last_refresh = p.last_refresh;
  }
}

#undef J
#undef J2
#undef JOPT
#undef JOPT2