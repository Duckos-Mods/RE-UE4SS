#pragma once
#include <cstdint>

#include <Common.hpp>
#include <GUI/GUI.hpp>
#include <glaze/glaze.hpp>
#include <glaze/glaze_exceptions.hpp>
#include <glaze/json.hpp>

struct staged_metadata_json
{
#pragma warning(disable : 4251)
    std::string overwritten_name{""};
    std::array<uint16_t, 3> mod_version{0, 0, 0};
    std::string description{""};
    std::string author{""};
    std::string main_file_name{""};

    std::map<std::string_view, std::map<std::string_view, std::array<uint16_t, 3>>> dependencies{};

    std::map<std::string_view, std::array<uint16_t, 3>> supported_ue4ss_version{};

#pragma warning(default : 4251)

    struct glaze
    {
        using T = staged_metadata_json;
        [[maybe_unused]] static constexpr std::string_view name = "staged_metadata_json";
		static constexpr auto value = glz::object(                             "name",
            																   &T::overwritten_name,
            																   "version",
            																   &T::mod_version,
            																   "description",
            																   &T::description,
            																   "author",
            																   &T::author,
            																   "main_file_name",
            																   &T::main_file_name,
            																   "dependencies",
            																   &T::dependencies,
            																   "ue4ss_version",
            																   &T::supported_ue4ss_version);
    };
};

namespace RC
{
    struct version
    {
        uint16_t Major{0};
        uint16_t Minor{0};
        uint16_t Patch{0};

        uint64_t to_uint64() const
        {
            uint64_t result{0};
            result |= Major;
            result <<= 16;
            result |= Minor;
            result <<= 16;
            result |= Patch;
            return result;
        }

        bool operator>(const version& other) const
        {
            if (Major > other.Major) return true;

            if (Minor > other.Minor) return true;

            if (Patch > other.Patch) return true;

            return false;
        }

        bool operator<(const version& other) const
        {
            if (Major < other.Major) return true;

            if (Minor < other.Minor) return true;

            if (Patch < other.Patch) return true;

            return false;
        }

        bool operator==(const version& other) const
        {
			if (Major != other.Major) return false;

			if (Minor != other.Minor) return false;

			if (Patch != other.Patch) return false;

			return true;
		}

        bool operator!=(const version& other) const
        {
			return !(*this == other);
		}
    };
    class RC_UE4SS_API DependencyData
    {
      public:
#pragma warning(disable : 4251)
        DependencyData() = default;
        std::wstring name{STR("")};
        std::pair<version, version> mod_version{{0, 0}, {0, 0}};
#pragma warning(default : 4251)
    };

    class RC_UE4SS_API ModMetadata
    {
      public:
#pragma warning(disable : 4251)
        version mod_version{99, 99, 99};        // We assume the mod is an insanly high version if it doesn't specify one to allow for easy version checking
        std::wstring overwritten_name{STR("")}; // Allows a mod to overwrite the name that UE4SS will use when displaying messages about the mod
        std::wstring description{STR("")};      // Simple description of the mod
        std::wstring author{STR("")};           // Author of the mod
        std::wstring main_file_name{STR("")};   // The name of the main file for the mod
        std::vector<DependencyData> dependencies{};
        std::pair<version, version> supported_ue4ss_version{version{0, 0, 0}, version{99, 99, 99}}; // The version range of UE4SS that this mod supports

        void process_metadata_file(const std::wstring& file_path);

#pragma warning(default : 4251)
      private:
        staged_metadata_json m_staged_metadata{};

      private:
        void transform_staged_metadata_to_metadata();
    };

};
