#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cstdint>

//Type aliases
using si32 = std::int32_t;
using ui32 = std::uint32_t;

si32 main(si32 t_argument_count, char* t_arguments[])
{
	//If the first argument is provided, use it as the input file path, otherwise use the one provided by the user
	std::string input_file_path;
	if (t_argument_count > 1)
	{
		input_file_path = std::string(t_arguments[1]);
	}
	else
	{
		std::cout << "DPM file path: ";
		std::cin >> input_file_path;
	}

	//Read and store the dpm data
	std::vector<char> dpm_data;
	std::ifstream input_file(input_file_path, std::ios::in | std::ios::binary | std::ios::ate);
	if (input_file.is_open())
	{
		dpm_data.resize(input_file.tellg());
		input_file.seekg(std::ios::beg);
		input_file.read(reinterpret_cast<char*>(dpm_data.data()), dpm_data.size());
		input_file.close();
	}
	else { std::exit(EXIT_FAILURE); }

	//Create a new folder for the extracted filess
	std::string output_folder_path = input_file_path + "-extracted\\";
	std::filesystem::create_directory(output_folder_path);

	//Get the header size
	ui32 header_size = *(reinterpret_cast<ui32*>(dpm_data.data() + 4));

	//Get the file count
	ui32 file_count = *(reinterpret_cast<ui32*>(dpm_data.data() + 8));

	for (ui32 i_file = 0; i_file < file_count; ++i_file)
	{
		//Read the name, offset and size from the header
		std::string name = std::string(dpm_data.data() + 16 + 32 * i_file, dpm_data.data() + 16 + 32 * i_file + 16);
		ui32 offset = *(reinterpret_cast<ui32*>(dpm_data.data() + 16 + 32 * i_file + 24));
		ui32 size = *(reinterpret_cast<ui32*>(dpm_data.data() + 16 + 32 * i_file + 28));

		//Write the contents to a separate file
		std::ofstream output_file(output_folder_path + name, std::ios::out | std::ios::binary);
		output_file.write(dpm_data.data() + header_size + offset, size);
		output_file.close();
	}

	//Exit
	return 0;
}