#pragma once

#include <chrono>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include <Common.hpp>
#include <File/File.hpp>
#include <Mod/ModMetadata.hpp>

namespace RC
{
    class UE4SSProgram;

    namespace Unreal
    {
        class UClass;
    }

    class RC_UE4SS_API Mod
    {
      public:
        UE4SSProgram& m_program;

      protected:
#pragma warning(disable : 4251)
        std::wstring m_mod_name;
        std::wstring m_mod_path;

        ModMetadata m_metadata; // We will init this in the constructor
#pragma warning(default : 4251)

      protected:
        // Whether the mod can be installed
        // This is true by default and is only false if the state of the mod won't allow for a successful installation
        bool m_installable{true};
        bool m_installed{false};
        bool m_is_startable{false};
        bool m_is_force_enabled{false};

        mutable bool m_is_started{false};

      public:
        enum class IsTrueMod
        {
            Yes,
            No
        };

      public:
        Mod(UE4SSProgram&, std::wstring&& mod_name, std::wstring&& mod_path);
        virtual ~Mod() = default;

      public:
        auto get_name() const -> std::wstring_view;

        virtual auto start_mod() -> void = 0;
        virtual auto uninstall() -> void = 0;

        auto set_installable(bool) -> void;
        auto is_installable() const -> bool;
        auto set_installed(bool) -> void;
        auto is_installed() const -> bool;
        auto is_started() const -> bool;
        auto get_metadata() -> ModMetadata&;
        auto is_startable() const -> bool;
        auto set_startable(bool) -> void;
        auto init_metadata() -> void;
        auto is_force_enabled() const -> bool;
        auto set_force_enabled(bool) -> void;



      public:
        // Main update from the program
        virtual auto fire_update() -> void;

        virtual auto fire_unreal_init() -> void{};

        // Called once when the program is starting, after mods are setup but before any mods have been started
        virtual auto fire_program_start() -> void{};

        // Async update
        // Used when the main update function would block other mods from executing their scripts
        virtual auto update_async() -> void;
    };
} // namespace RC
