#pragma once


#include <imgui.h>
#include <imgui_internal.h>
#include <chrono>
#include <string>
#include <filesystem>
#include <sstream>

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

	void ShowFileDialog(bool* open, char* buffer, unsigned int bufferSize, FileDialogType type = FileDialogType::OpenFile) {
		static int fileDialogFileSelectIndex = 0;
		static int fileDialogFolderSelectIndex = 0;
		static std::string fileDialogCurrentPath = "C:\\dev\\";
		static std::string fileDialogCurrentFile = "";
		static std::string fileDialogCurrentFolder = "";
		static char fileDialogError[500] = "";
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

			std::string selectedFilePath = fileDialogCurrentPath + (fileDialogCurrentPath.back() == '\\' ? "" : "\\") + (fileDialogCurrentFolder.size() > 0 ? fileDialogCurrentFolder : fileDialogCurrentFile);
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
						std::string newFilePath = fileDialogCurrentPath + (fileDialogCurrentPath.back() == '\\' ? "" : "\\") + newFolderName;
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
				ImGui::TextUnformatted(fileDialogCurrentFolder.c_str());
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
				if (ImGui::Button("Yes")) {
					std::filesystem::remove(fileDialogCurrentPath + (fileDialogCurrentPath.back() == '\\' ? "" : "\\") + fileDialogCurrentFolder);
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
						strcpy_s(fileDialogError, "Error: You must select a folder!");
					}
					else {
						strcpy(buffer, (fileDialogCurrentPath + (fileDialogCurrentPath.back() == '\\' ? "" : "\\") + fileDialogCurrentFolder).c_str());
						fileDialogFileSelectIndex = 0;
						fileDialogFolderSelectIndex = 0;
						fileDialogCurrentFile = "";
						fileDialogOpen = false;
					}
				}
			}

			if (strlen(fileDialogError) > 0) {
				ImGui::TextColored(ImColor(1.0f, 0.0f, 0.2f, 1.0f), fileDialogError);
			}

			ImGui::End();
		}
	}

}

