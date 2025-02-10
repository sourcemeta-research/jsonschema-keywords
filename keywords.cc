#include <iostream>
#include <iomanip>
#include <filesystem>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <utility>

#include <sourcemeta/core/yaml.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

using Summary = std::map<std::pair<std::string, std::string>, std::uint64_t>;

auto process_schema(const sourcemeta::core::JSON &schema, Summary &summary) -> void {
  for (const auto &entry :
           sourcemeta::core::SchemaIterator{
           schema, sourcemeta::core::schema_official_walker,
           sourcemeta::core::schema_official_resolver}) {
    if (!entry.subschema.get().is_object()) {
      continue;
    }

    for (const auto &property : entry.subschema.get().as_object()) {
      const auto walker_result{sourcemeta::core::schema_official_walker(property.first, entry.vocabularies)};
      const auto vocabulary{walker_result.vocabulary.value_or("none")};
      const std::pair<std::string, std::string> key{vocabulary, property.first};
      summary[key] += 1;
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
      std::cerr << "Usage: " << argv[0] << " <schemas...>" << std::endl;
      return 1;
  }

  Summary summary;

  for (int i = 1; i < argc; i++) {
    std::filesystem::path file_path{argv[i]};
    if (file_path.extension() == ".json") {
      std::cerr << "Processing as JSON: " << file_path.string() << '\n';
      process_schema(sourcemeta::core::read_json(file_path), summary);
    } else if (file_path.extension() == ".yaml" || file_path.extension() == ".yml") {
      std::cerr << "Processing as YAML: " << file_path.string() << '\n';
      process_schema(sourcemeta::core::read_yaml(file_path), summary);
    } else {
      std::cerr << "File is not a schema: " << file_path.string() << '\n';
      return 1;
    }
  }

  std::vector<std::pair<std::pair<std::string, std::string>, std::uint64_t>> 
    vec(summary.begin(), summary.end());
  std::sort(vec.begin(), vec.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

  for (const auto &entry : vec) {
    std::cout << std::setw(5) 
              << entry.second 
              << " - " 
              << entry.first.second 
              << " (" << entry.first.first << ")\n";
  }

  return 0;
}
