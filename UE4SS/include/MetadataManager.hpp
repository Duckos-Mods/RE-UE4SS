#pragma once
#include <cstdint>

#include <Common.hpp>
#include <File/File.hpp>
#include <GUI/GUI.hpp>
#include <Mod/Mod.hpp>


namespace RC
{
    class RC_UE4SS_API MetadataManager
    {
        public:

            auto manage_dependencies(std::unique_ptr<Mod>& mod, std::vector<std::unique_ptr<Mod>>& mods) -> bool;
            auto get_mod_index(std::wstring& mod_name) -> unsigned int;
            auto add_to_mnti(std::wstring& mod_name, unsigned int index) -> void;
            auto verify_mnti(std::vector<std::unique_ptr<Mod>>& mods) -> void;

        private:
#pragma warning(disable : 4251)
            std::map<std::wstring, unsigned int> m_mod_name_to_index{}; // We have this map so we can quickly find the index of a mod in the vector with out calling find_mod_by_name this isnt used for anything else
#pragma warning(default : 4251)
          // Theoretically we could replace the find_mod_by_name_internal function with this and it would be much faster but I dont want to break anything
            // I might toy around with that later though and see if it works out better than the current system 
            // For now this is just used for managing dependencies

        private:
            auto swap_mods(std::vector<std::unique_ptr<Mod>>& mods, std::wstring& modOne, std::wstring& modTwo) -> void;
            auto mod_is_valid_version(std::unique_ptr<Mod>& dependancy, DependencyData& expected) -> bool;
            auto manage_mod_position(std::unique_ptr<Mod>& dependant,
                                     std::unique_ptr<Mod>& dependency,
                                     std::vector<std::unique_ptr<Mod>>& mods)
                    -> bool;

            auto brute_force_correct_mnti(std::vector<std::unique_ptr<Mod>>& mods) -> void;
    };
}