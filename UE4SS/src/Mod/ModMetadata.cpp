#include <Mod/ModMetadata.hpp>
#include <JSON/JSON.hpp>


namespace RC
{
    auto ModMetadata::process_metadata_file(const std::wstring& path) -> void
    {

        // Open the file
        std::ifstream file(std::string(path.begin(), path.end()));

        // Check if the file is open
        if (!file.is_open())
        {
            return;
        }

        // Read the file contents into a string
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonContent = buffer.str();

        auto json = glz::read_json<staged_metadata_json>(jsonContent);
        m_staged_metadata = json.value();
        transform_staged_metadata_to_metadata();
    }

    auto ModMetadata::transform_staged_metadata_to_metadata() -> void
    {
        // Check if the author decided to overwrite the mod name and use that instead
        if (!m_staged_metadata.overwritten_name.empty())
        {
            overwritten_name = m_staged_metadata.overwritten_name;
        }

        // Makes sure that the mod has a version number
        mod_version = version{m_staged_metadata.mod_version[0], m_staged_metadata.mod_version[1], m_staged_metadata.mod_version[2]};

        // Asign the description
        description = m_staged_metadata.description;
    
        // Asign the author
        author = m_staged_metadata.author;

        // Asign the main file name
        main_file_name = m_staged_metadata.main_file_name;

        // Asign the UE4SS versions lambda
        auto versionLambda = [&](std::map<std::string_view, std::array<uint16_t, 3>>& versionMap) const -> auto {
            std::pair<version, version> versions{};
            versions.second = version{99, 99, 99};
            for (auto& version_data : versionMap)
            {
                if (version_data.first == "min")
                {
                    versions.first = version(version_data.second[0], version_data.second[1], version_data.second[2]);
				}
                else if (version_data.first == "max")
                {
                    versions.second = version(version_data.second[0], version_data.second[1], version_data.second[2]);
				}
            }
            return versions;
        };
        
        supported_ue4ss_version = versionLambda(m_staged_metadata.supported_ue4ss_version);

        // Asign the dependencies lambda

        static auto dependenciesLambda = [&](std::vector<DependencyData>& depsVector) -> void {
                for (auto& [depName, depVersionData] : m_staged_metadata.dependencies)
                {
                    DependencyData depData{};

                    // Assign the name data
                    depData.name = std::wstring(depName.begin(), depName.end());

                    depData.mod_version = versionLambda(depVersionData);
                }

            };

        dependenciesLambda(dependencies);
    
    }

}