/*
	Copyright 2020 Limeoats

   	Licensed under the Apache License, Version 2.0 (the "License");
   	you may not use this file except in compliance with the License.
   	You may obtain a copy of the License at
	
       	http://www.apache.org/licenses/LICENSE-2.0
	
   	Unless required by applicable law or agreed to in writing, software
   	distributed under the License is distributed on an "AS IS" BASIS,
   	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   	See the License for the specific language governing permissions and
   	limitations under the License.
*/

#pragma once


#include <imgui.h>
#include <imgui_internal.h>
#include <chrono>
#include <string>
#include <filesystem>
#include <sstream>
#include <vector>


using namespace std::chrono_literals;

namespace FileDialog {

	enum class FileDialogType {
		OpenFile,
		SelectFolder
	};
	enum class FileDialogSortOrder {
		Up,
		Down,
		None
	};

	static bool fileDialogOpen = false;

	void ShowFileDialog(bool* open, std::filesystem::path& buffer, FileDialogType type = FileDialogType::OpenFile) {
		static int fileDialogFileSelectIndex = 0;
		static int fileDialogFolderSelectIndex = 0;
		static std::filesystem::path fileDialogCurrentPath = std::filesystem::current_path();
		static std::filesystem::path fileDialogCurrentFile;
		static std::filesystem::path fileDialogCurrentFolder;
		static std::string fileDialogError = "";
		static FileDialogSortOrder fileNameSortOrder = FileDialogSortOrder::None;
		static FileDialogSortOrder sizeSortOrder = FileDialogSortOrder::None;
		static FileDialogSortOrder dateSortOrder = FileDialogSortOrder::None;
		static FileDialogSortOrder typeSortOrder = FileDialogSortOrder::None;

		if (open) {
			ImGui::SetNextWindowSize(ImVec2(740.0f, 410.0f));
			ImGui::Begin("Select a file", nullptr, ImGuiWindowFlags_NoResize);

			std::vector<std::filesystem::directory_entry> files;
			std::vector<std::filesystem::directory_entry> folders;
			try {
				for (auto& p : std::filesystem::directory_iterator(fileDialogCurrentPath)) {
					if (p.is_directory()) {
						folders.push_back(p);
					}
					else {
						files.push_back(p);
					}
				}
			}
			catch (...) {}

			ImGui::Text("%s", fileDialogCurrentPath.c_str());

			ImGui::BeginChild("Directories##1", ImVec2(200, 300), true, ImGuiWindowFlags_HorizontalScrollbar);

			if (ImGui::Selectable("..", false, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
				if (ImGui::IsMouseDoubleClicked(0)) {
					fileDialogCurrentPath = std::filesystem::path(fileDialogCurrentPath).parent_path().string();
				}
			}
			for (int i = 0; i < folders.size(); ++i) {
				if (ImGui::Selectable(folders[i].path().stem().string().c_str(), i == fileDialogFolderSelectIndex, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
					fileDialogCurrentFile = "";
					if (ImGui::IsMouseDoubleClicked(0)) {
						fileDialogCurrentPath = folders[i].path().string();
						fileDialogFolderSelectIndex = 0;
						fileDialogFileSelectIndex = 0;
						ImGui::SetScrollHereY(0.0f);
						fileDialogCurrentFolder = "";
					}
					else {
						fileDialogFolderSelectIndex = i;
						fileDialogCurrentFolder = folders[i].path().stem().string();
					}
				}
			}
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginChild("Files##1", ImVec2(516, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::Columns(4);
			static float initialSpacingColumn0 = 230.0f;
			if (initialSpacingColumn0 > 0) {
				ImGui::SetColumnWidth(0, initialSpacingColumn0);
				initialSpacingColumn0 = 0.0f;
			}
			static float initialSpacingColumn1 = 80.0f;
			if (initialSpacingColumn1 > 0) {
				ImGui::SetColumnWidth(1, initialSpacingColumn1);
				initialSpacingColumn1 = 0.0f;
			}
			static float initialSpacingColumn2 = 80.0f;
			if (initialSpacingColumn2 > 0) {
				ImGui::SetColumnWidth(2, initialSpacingColumn2);
				initialSpacingColumn2 = 0.0f;
			}
			if (ImGui::Selectable("File")) {
				sizeSortOrder = FileDialogSortOrder::None;
				dateSortOrder = FileDialogSortOrder::None;
				typeSortOrder = FileDialogSortOrder::None;
				fileNameSortOrder = (fileNameSortOrder == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
			}
			ImGui::NextColumn();
			if (ImGui::Selectable("Size")) {
				fileNameSortOrder = FileDialogSortOrder::None;
				dateSortOrder = FileDialogSortOrder::None;
				typeSortOrder = FileDialogSortOrder::None;
				sizeSortOrder = (sizeSortOrder == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
			}
			ImGui::NextColumn();
			if (ImGui::Selectable("Type")) {
				fileNameSortOrder = FileDialogSortOrder::None;
				dateSortOrder = FileDialogSortOrder::None;
				sizeSortOrder = FileDialogSortOrder::None;
				typeSortOrder = (typeSortOrder == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
			}
			ImGui::NextColumn();
			if (ImGui::Selectable("Date")) {
				fileNameSortOrder = FileDialogSortOrder::None;
				sizeSortOrder = FileDialogSortOrder::None;
				typeSortOrder = FileDialogSortOrder::None;
				dateSortOrder = (dateSortOrder == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
			}
			ImGui::NextColumn();
			ImGui::Separator();

			// Sort files
			if (fileNameSortOrder != FileDialogSortOrder::None) {
				std::sort(files.begin(), files.end(), [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
					if (fileNameSortOrder == FileDialogSortOrder::Down) {
						return a.path().filename().string() > b.path().filename().string();
					}
					else {
						return a.path().filename().string() < b.path().filename().string();
					}
					});
			}
			else if (sizeSortOrder != FileDialogSortOrder::None) {
				std::sort(files.begin(), files.end(), [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
					if (sizeSortOrder == FileDialogSortOrder::Down) {
						return a.file_size() > b.file_size();
					}
					else {
						return a.file_size() < b.file_size();
					}
					});
			}
			else if (typeSortOrder != FileDialogSortOrder::None) {
				std::sort(files.begin(), files.end(), [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
					if (typeSortOrder == FileDialogSortOrder::Down) {
						return a.path().extension().string() > b.path().extension().string();
					}
					else {
						return a.path().extension().string() < b.path().extension().string();
					}
					});
			}
			else if (dateSortOrder != FileDialogSortOrder::None) {
				std::sort(files.begin(), files.end(), [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
					if (dateSortOrder == FileDialogSortOrder::Down) {
						return a.last_write_time() > b.last_write_time();
					}
					else {
						return a.last_write_time() < b.last_write_time();
					}
					});
			}

			for (int i = 0; i < files.size(); ++i) {
				if (ImGui::Selectable(files[i].path().filename().string().c_str(), i == fileDialogFileSelectIndex, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
					fileDialogFileSelectIndex = i;
					fileDialogCurrentFile = files[i].path().filename().string();
					fileDialogCurrentFolder = "";
				}
				ImGui::NextColumn();
				ImGui::TextUnformatted(std::to_string(files[i].file_size()).c_str());
				ImGui::NextColumn();
				ImGui::TextUnformatted(files[i].path().extension().string().c_str());
				ImGui::NextColumn();
				auto ftime = files[i].last_write_time();
				auto st = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
				std::time_t tt = std::chrono::system_clock::to_time_t(st);
				std::tm* mt = std::localtime(&tt);
				std::stringstream ss;
				ss << std::put_time(mt, "%F %R");
				ImGui::TextUnformatted(ss.str().c_str());
				ImGui::NextColumn();
			}
			ImGui::EndChild();

			std::string selectedFilePath = fileDialogCurrentPath.generic_string();
			char* buf = &selectedFilePath[0];
			ImGui::PushItemWidth(724);
			ImGui::InputText("", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);

			if (ImGui::Button("New folder")) {
				ImGui::OpenPopup("NewFolderPopup");
			}
			ImGui::SameLine();

			static bool disableDeleteButton = false;
			disableDeleteButton = (fileDialogCurrentFolder == "");
			if (disableDeleteButton) {
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			if (ImGui::Button("Delete folder")) {
				ImGui::OpenPopup("DeleteFolderPopup");
			}
			if (disableDeleteButton) {
				ImGui::PopStyleVar();
				ImGui::PopItemFlag();
			}

			ImVec2 center(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x * 0.5f, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y * 0.5f);
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopup("NewFolderPopup", ImGuiWindowFlags_Modal)) {
				ImGui::Text("Enter a name for the new folder");
				static char newFolderName[500] = "";
				static char newFolderError[500] = "";
				ImGui::InputText("", newFolderName, sizeof(newFolderName));
				if (ImGui::Button("Create##1")) {
					if (strlen(newFolderName) <= 0) {
						strcpy_s(newFolderError, "Folder name can't be empty");
					}
					else {
						auto newFilePath = fileDialogCurrentPath / newFolderName;
						std::filesystem::create_directory(newFilePath);
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel##1")) {
					strcpy_s(newFolderName, "");
					strcpy_s(newFolderError, "");
					ImGui::CloseCurrentPopup();
				}
				ImGui::TextColored(ImColor(1.0f, 0.0f, 0.2f, 1.0f), newFolderError);
				ImGui::EndPopup();
			}

			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopup("DeleteFolderPopup", ImGuiWindowFlags_Modal)) {
				ImGui::TextColored(ImColor(1.0f, 0.0f, 0.2f, 1.0f), "Are you sure you want to delete this folder?");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
				ImGui::TextUnformatted(fileDialogCurrentFolder.generic_string().c_str());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
				if (ImGui::Button("Yes")) {
					std::filesystem::remove(fileDialogCurrentPath/fileDialogCurrentFolder);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("No")) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 120);

			if (ImGui::Button("Cancel")) {
				fileDialogFileSelectIndex = 0;
				fileDialogFolderSelectIndex = 0;
				fileDialogCurrentFile = "";
				fileDialogOpen = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Choose")) {
				if (type == FileDialogType::SelectFolder) {
					if (fileDialogCurrentFolder == "") {
						fileDialogError="Error: You must select a folder!";
					}
					else {
						buffer=fileDialogCurrentPath/ fileDialogCurrentFolder;
						fileDialogFileSelectIndex = 0;
						fileDialogFolderSelectIndex = 0;
						fileDialogCurrentFile = "";
						fileDialogOpen = false;
					}
				}
				else {
					if (fileDialogCurrentFile.empty()) {
						fileDialogError="Error: You must select a file!";
					}
					else {
						buffer = fileDialogCurrentPath/fileDialogCurrentFile ;
						fileDialogOpen = false;
						fileDialogFileSelectIndex = 0;
						fileDialogFolderSelectIndex = 0;
						fileDialogCurrentFile = "";
					}
				}
			}

			if (fileDialogError.size() > 0) {
				ImGui::TextColored(ImColor(1.0f, 0.0f, 0.2f, 1.0f), &fileDialogError[0]);
			}
			ImGui::End();
		}
	}

	void ShowFileDialog(bool* open, std::string& buffer, FileDialogType type = FileDialogType::OpenFile) {
		std::filesystem::path path;
		ShowFileDialog(open, path, type);
		buffer=path.generic_string();
	}
}

