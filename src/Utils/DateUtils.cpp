#include "Utils/DateUtils.hpp"

#include "custom-types/shared/util.hpp"

std::string SongInfoPlus::DateUtils::getMonthFromString(std::string s) {
  if (s.starts_with("01")) {
    return "January";
  } else if (s.starts_with("02")) {
    return "Febuary";
  } else if (s.starts_with("03")) {
    return "March";
  } else if (s.starts_with("04")) {
    return "April";
  } else if (s.starts_with("05")) {
    return "May";
  } else if (s.starts_with("06")) {
    return "June";
  } else if (s.starts_with("07")) {
    return "July";
  } else if (s.starts_with("08")) {
    return "August";
  } else if (s.starts_with("09")) {
    return "September";
  } else if (s.starts_with("10")) {
    return "October";
  } else if (s.starts_with("11")) {
    return "November";
  } else if (s.starts_with("12")) {
    return "December";
  }
  return "Month";
}

std::string SongInfoPlus::DateUtils::getNumberEnd(std::string number) {
  if (number.ends_with("0") || number.ends_with("4") || number.ends_with("5") ||
      number.ends_with("6") || number.ends_with("7") || number.ends_with("8") ||
      number.ends_with("9"))
    return "th";
  if (number.ends_with("1")) return "st";
  if (number.ends_with("12")) return "th";
  if (number.ends_with("2")) return "nd";
  if (number.ends_with("3")) return "rd";
  return "th";
}

std::string SongInfoPlus::DateUtils::getReadableDate(std::string s) {
  Il2CppString *s2 = il2cpp_utils::newcsstr(s);
  ::Array<::Il2CppString *> *array = s2->Split('-');

  Il2CppString *day = array->get(2)->Split('T')->get(0);
  Il2CppString *year = array->get(0);
  Il2CppString *month = array->get(1);

  std::string day2 = to_utf8(csstrtostr(day));

  if (day2.starts_with("0")) {
    day2 = day2.substr(1);
  }

  return getMonthFromString(to_utf8(csstrtostr(month))) + " " + day2 +
         getNumberEnd(to_utf8(csstrtostr(day))) + ", " +
         to_utf8(csstrtostr(year));
}