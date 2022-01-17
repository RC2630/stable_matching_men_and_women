#include "general/vectorUtility.h"
#include <map>

using namespace std;

struct Person {
  string name;
  bool isFree;
  map<string, int> prefList;
};

struct Woman; // forward declaration

struct Man : public Person {
  vector<string> proposedList;
  Man(const string& name_, const map<string, int>& prefList_);
  Woman& highestRankedUnproposedWoman(vector<Woman>& women) const;
};

struct Woman : public Person {
  Woman(const string& name_, const map<string, int>& prefList_);
  Man& currentlyEngagedTo(const vector<pair<Man, Woman>>& rsf, vector<Man>& men) const;
  bool prefers(const Man& man1, const Man& man2) const;
};

Man::Man(const string& name_, const map<string, int>& prefList_) {
  name = name_;
  isFree = true;
  prefList = prefList_;
  proposedList = {};
}

Woman::Woman(const string& name_, const map<string, int>& prefList_) {
  name = name_;
  isFree = true;
  prefList = prefList_;
}

Woman& Man::highestRankedUnproposedWoman(vector<Woman>& women) const {
  string highestName = "uninitialized";
  int highestScore = 0;
  for (auto woman : prefList) {
    if (woman.second > highestScore && !vecUtil::contains(proposedList, woman.first)) {
      highestScore = woman.second;
      highestName = woman.first;
    }
  }
  for (Woman& woman : women) {
    if (woman.name == highestName) {
      return woman;
    }
  }
  throw '!';
}

Man& Woman::currentlyEngagedTo(const vector<pair<Man, Woman>>& rsf, vector<Man>& men) const {
  string partnerName = "uninitialized";
  for (auto& couple : rsf) {
    if (couple.second.name == name) {
      partnerName = couple.first.name;
      break;
    }
  }
  for (Man& man : men) {
    if (man.name == partnerName) {
      return man;
    }
  }
  throw '!';
}

bool Woman::prefers(const Man& man1, const Man& man2) const {
  return prefList.at(man1.name) > prefList.at(man2.name);
}

bool thereisFreeMan(const vector<Man>& men) {
  for (Man man : men) {
    if (man.isFree) {
      return true;
    }
  }
  return false;
}

Man& nextFreeMan(vector<Man>& men) {
  for (Man& man : men) {
    if (man.isFree) {
      return man;
    }
  }
  throw '!';
}

bool operator == (const pair<Man, Woman>& couple1, const pair<Man, Woman>& couple2) {
  return couple1.first.name == couple2.first.name && couple1.second.name == couple2.second.name;
}

vector<pair<Man, Woman>> stableMatch(vector<Man>& men, vector<Woman>& women) {
  vector<pair<Man, Woman>> result;
  while (thereisFreeMan(men)) {
    Man& man = nextFreeMan(men);
    Woman& woman = man.highestRankedUnproposedWoman(women);
    man.proposedList.push_back(woman.name);
    if (woman.isFree) {
      man.isFree = false;
      woman.isFree = false;
      result.push_back({man, woman});
    } else {
      Man& otherMan = woman.currentlyEngagedTo(result, men);
      if (woman.prefers(otherMan, man)) {
        // nothing happens
      } else {
        otherMan.isFree = true;
        man.isFree = false;
        vecUtil::removeFirstInstance(result, {otherMan, woman});
        result.push_back({man, woman});
      }
    }
  }
  return result;
}

ostream& operator << (ostream& out, const pair<Man, Woman>& couple) {
  out << couple.first.name << " & " << couple.second.name;
  return out;
}

int main() {

  Man arthur("Arthur", {{"Orlanda", 87}, {"Penny", 56}, {"Quartz", 71}, {"Rosa", 61}});
  Man brandon("Brandon", {{"Orlanda", 16}, {"Penny", 47}, {"Quartz", 89}, {"Rosa", 77}});
  Man charles("Charles", {{"Orlanda", 52}, {"Penny", 78}, {"Quartz", 91}, {"Rosa", 55}});
  Man david("David", {{"Orlanda", 73}, {"Penny", 74}, {"Quartz", 65}, {"Rosa", 82}});

  Woman orlanda("Orlanda", {{"Arthur", 71}, {"Brandon", 64}, {"Charles", 88}, {"David", 74}});
  Woman penny("Penny", {{"Arthur", 67}, {"Brandon", 91}, {"Charles", 56}, {"David", 41}});
  Woman quartz("Quartz", {{"Arthur", 75}, {"Brandon", 85}, {"Charles", 88}, {"David", 83}});
  Woman rosa("Rosa", {{"Arthur", 18}, {"Brandon", 63}, {"Charles", 86}, {"David", 94}});

  vector<Man> men = {arthur, brandon, charles, david};
  vector<Woman> women = {orlanda, penny, quartz, rosa};

  using vecUtil::operator<<;
  cout << "\nmatch result = " << stableMatch(men, women) << "\n\n";

}