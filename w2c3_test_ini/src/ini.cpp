#include "ini.h"

using namespace std;

namespace Ini {

Section& Document::AddSection(string name) {
  return Document::sections[move(name)];
}

const Section& Document::GetSection(const string& name) const {
  return Document::sections.at(name);
}

size_t Document::SectionCount() const {
  return Document::sections.size();
}

namespace IniPrivate {
pair<string, string> LoadPair (istream& input) {
  string key, value;
  getline(input, key, '=');
  getline(input, value);
  return {move(key), move(value)};
}
}

Document Load(istream& input) {
  Document result;
  Section* section;
  for (char c; input >> c;) {
    if (c == '[') {
      string section_name;
      getline(input, section_name, ']');
      section = &result.AddSection(section_name);
      input.ignore(1);
    } else {
      input.putback(c);
      section->insert(IniPrivate::LoadPair(input));
    }
  }
  return move(result);
}

}
