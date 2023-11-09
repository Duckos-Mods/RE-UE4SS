#include <MetadataManager.hpp>
#include <Mod/Mod.hpp>

namespace RC
{
	auto MetadataManager::manage_mod_position(std::unique_ptr<Mod>& dependant, std::unique_ptr<Mod>& dependency, std::vector<std::unique_ptr<Mod>>& mods)
            -> bool
    {
		// get mod names
		std::wstring dependantName = dependant->get_name().data();
        std::wstring dependencyName = dependency->get_name().data();


		// Check the positions of the mods in the array
        size_t dependantIndex = m_mod_name_to_index[dependantName];
        size_t dependencyIndex = m_mod_name_to_index[dependencyName];

		// Check if the dependency has a lower index than the dependant if so return
		if (dependencyIndex < dependantIndex)
            return false;

		// Swap the mods	
        swap_mods(mods, dependantName, dependencyName);
        return true;
    }

	auto MetadataManager::mod_is_valid_version(std::unique_ptr<Mod>& dependency, DependencyData& expected) -> bool 
	{
		// Check if a max version is specified
		bool maxVersionSpecified = expected.mod_version.second.to_uint64() != 0;
		if (maxVersionSpecified)
		{
			// Check if the dependency version is greater than the max version
			if (dependency->get_metadata().mod_version > expected.mod_version.second)
			{
				return false;
			}
		}
		// Check if a min version is specified
		bool minVersionSpecified = expected.mod_version.first.to_uint64() != 0;
		if (minVersionSpecified)
		{
			// Check if the dependency version is less than the min version
			if (dependency->get_metadata().mod_version < expected.mod_version.first)
			{
				return false;
			}
		}
        return true;
	}

	void MetadataManager::swap_mods(std::vector<std::unique_ptr<Mod>>& mods, std::wstring& modOne, std::wstring& modTwo)
	{
		// Error checking
		if (m_mod_name_to_index.find(modOne) == m_mod_name_to_index.end() || m_mod_name_to_index.find(modTwo) == m_mod_name_to_index.end())
		{
            Output::send<LogLevel::Error>(L"Mod name not found in mod name to index map");
            return;
		}

		// Handle swapping of mods in the vector
		unsigned int startIndex = m_mod_name_to_index[modOne];
		unsigned int endIndex = m_mod_name_to_index[modTwo];
		mods[startIndex].swap(mods[endIndex]);
        m_mod_name_to_index[modOne] = endIndex;
		m_mod_name_to_index[modTwo] = startIndex;
	}

    auto MetadataManager::manage_dependencies(std::unique_ptr<Mod>& mod, std::vector<std::unique_ptr<Mod>>& mods) -> bool
    {
        // Check for dependencies
        if (mod->get_metadata().dependencies.empty())
        {
            return false;
        }
        bool modifiedArray = false;

        for (auto& dependency : mod->get_metadata().dependencies)
        {
            // Check if mod exists
			auto it = m_mod_name_to_index.find(dependency.name);
            if (it == m_mod_name_to_index.end())
            {
                Output::send<LogLevel::Error>(STR("Mod dependency not found: {}"), dependency.name);
                break;
            }

			size_t dependencyIndex = it->second;

            // Do Version check
            if (!mod_is_valid_version(mods[dependencyIndex], dependency))
            {
                Output::send<LogLevel::Warning>(STR("Mod dependency version mismatch: {}"), dependency.name);
                Output::send<LogLevel::Warning>(STR("Proceeding with loading but there is a chance some mods may not work correctly!\n"));
            }

			// Tell the mod loader to load the dependency even if it is disabled
            mods[dependencyIndex]->set_force_enabled(true);
			// Or the result of manage_mod_position with modifiedArray to see if any call modified the array
            modifiedArray |= manage_mod_position(mod, mods[dependencyIndex], mods);
        }
		return modifiedArray;
    }

	auto MetadataManager::get_mod_index(std::wstring& mod_name) -> unsigned int
	{
		if (m_mod_name_to_index.find(mod_name) == m_mod_name_to_index.end())
		{
			Output::send<LogLevel::Error>(L"Mod name not found in mod name to index map");
			return 0;
		}
		return m_mod_name_to_index[mod_name];
	}

	auto MetadataManager::add_to_mnti(std::wstring& mod_name, unsigned int index) -> void
	{
		m_mod_name_to_index[mod_name] = index;
	}

	auto MetadataManager::brute_force_correct_mnti(std::vector<std::unique_ptr<Mod>>& mods)
		->void
	{
        int current_index = 0;
		for (auto& mod : mods)
		{
            m_mod_name_to_index[mod->get_name().data()] = current_index;
			current_index++;
		}
	}

    auto MetadataManager::verify_mnti(std::vector<std::unique_ptr<Mod>>& mods) -> void
	{
        for (auto& [name, index] : m_mod_name_to_index)
		{
            if (name != mods[index]->get_name().data())
			{
				Output::send<LogLevel::Error>(STR("Mod name to index map contains invalid index: {}"), name);
                Output::send<LogLevel::Warning>(STR("Attempting a brute force to correct the map!\n"));
                brute_force_correct_mnti(mods);
				Output::send<LogLevel::Normal>(STR("Brute force correction complete! Verifying!\n"));
				verify_mnti(mods);

				// This function has a chance of causing an infinite loop but that should never happen unless there is memory corruption messing with the map/array
			}
		}
	}
}